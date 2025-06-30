#include "FpsAdjustPipe.h"

void video::FpsAdjustPipe::ReadAndSendFrame()
{
	AVFrameWrapper frame;
	bool have_synced = false;
	int64_t delta_pts = 0;

	while (true)
	{
		bool result = _graph.ReadData(frame);
		if (!result)
		{
			return;
		}

		frame.SetTimeBase(AVRational{_desired_out_fps.den, _desired_out_fps.num});
		frame.ChangeTimeBase(AVRational{1, 90000});

		// 将读取的帧的时间戳与 _ref_pts 对齐。
		if (!have_synced)
		{
			int64_t ref_pts = _trigger.GetOutput();

			/* 将触发器的输出更新到输入。
			 * 从 fps 滤镜输出端读取帧成功的那一刻，fps 滤镜输入端送入的那个帧就是下一次
			 * 读取成功时要同步 pts 的对象。
			 */
			_trigger.UpdateOutput();
			delta_pts = ref_pts - frame.Pts();
			have_synced = true;
		}

		// 滤镜出来的 pts 与输入端的 pts 有误差，则本轮循环读取的每一个帧的 pts 都要加上 delta_pts。
		frame.SetPts(frame.Pts() + delta_pts);
		frame.ChangeTimeBase(_input_video_stream_infos.TimeBase());
		SendDataToEachConsumer(frame);
	}
}

video::FpsAdjustPipe::FpsAdjustPipe(IVideoStreamInfoCollection const &input_video_stream_infos,
									AVRational const &desired_out_fps)
	: _input_video_stream_infos(input_video_stream_infos),
	  _graph(input_video_stream_infos),
	  _desired_out_fps(desired_out_fps)
{
	AVFilterContextWrapper fps_filter = _graph.alloc_fps_filter(desired_out_fps);

	// 连接滤镜
	_graph.buffer_filter() << fps_filter << _graph.buffer_sink_filter();

	// 配置滤镜图
	_graph.config_graph();
}

void video::FpsAdjustPipe::SendData(AVFrameWrapper &frame)
{
	if (ConsumerList().Count() == 0)
	{
		return;
	}

	ReadAndSendFrame();
	frame.ChangeTimeBase(AVRational{1, 90000});
	_trigger.UpdateInput(frame.Pts());
	if (_trigger.HaveNotUpdateOutput())
	{
		// 第一次输入数据，让输入的数据直通到输出端
		_trigger.UpdateOutput();
	}

	_graph.SendData(frame);
	ReadAndSendFrame();
}

void video::FpsAdjustPipe::Flush()
{
	if (ConsumerList().Count() == 0)
	{
		return;
	}

	ReadAndSendFrame();
	_graph.Flush();
	ReadAndSendFrame();
	FlushEachConsumer();
}
