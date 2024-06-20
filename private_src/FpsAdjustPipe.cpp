#include"FpsAdjustPipe.h"

using namespace video;
using namespace std;

void FpsAdjustPipe::ReadAndSendFrame()
{
	AVFrameWrapper frame;
	bool have_synced = false;
	int64_t delta_pts = 0;

	while (1)
	{
		int ret = _graph.ReadFrame(frame);
		switch (ret)
		{
		case 0:
			{
				// 将读取的帧的时间戳与 _ref_pts 对齐。
				if (!have_synced)
				{
					int64_t ref_pts = _trigger.GetOutput();

					/* 将触发器的输出更新到输入。
					* 从 fps 滤镜输出端读取帧成功的那一刻，fps 滤镜输入端送入的那个帧就是下一次
					* 读取成功时要同步 pts 的对象。
					*/
					_trigger.UpdateOutput();
					delta_pts = ref_pts - frame.pts();
					have_synced = true;
				}

				// 滤镜出来的 pts 与输入端的 pts 有误差，则本轮循环读取的每一个帧的 pts 都要加上 delta_pts。
				frame.set_pts(frame.pts() + delta_pts);
				frame.SetTimeBase(AVRational{ _desired_out_fps.den, _desired_out_fps.num });
				SendFrameToEachConsumer(&frame);

				// 下轮循环继续读取
				break;
			}
		case (int)ErrorCode::output_is_temporarily_unavailable:
			{
				return;
			}
		case (int)ErrorCode::eof:
			{
				SendFrameToEachConsumer(nullptr);
				return;
			}
		}
	}
}

FpsAdjustPipe::FpsAdjustPipe(IVideoStreamInfoCollection const &input_video_stream_infos, AVRational desired_out_fps) :
	_input_video_stream_infos(input_video_stream_infos),
	_graph(input_video_stream_infos),
	_desired_out_fps(desired_out_fps)
{
	auto fps_filter = _graph.alloc_fps_filter(desired_out_fps);

	// 连接滤镜
	_graph.buffer_filter() << fps_filter << _graph.buffer_sink_filter();

	// 配置滤镜图
	_graph.config_graph();
}

void FpsAdjustPipe::SendFrame(AVFrameWrapper *frame)
{
	if (FrameConsumerList().Count() == 0)
	{
		return;
	}

	ReadAndSendFrame();
	if (frame != nullptr)
	{
		frame->ChangeTimeBase(_input_video_stream_infos.TimeBase());
	}

	_graph.SendFrame(frame);

	if (frame)
	{
		int64_t rescaled_current_frame_pts = ConvertTimeStamp(
			frame->pts(),
			frame->TimeBase(),
			AVRational{ _desired_out_fps.den, _desired_out_fps.num }
		);

		_trigger.UpdateInput(rescaled_current_frame_pts);
		if (_trigger.HaveNotUpdateOutput())
		{
			// 第一次输入数据，让输入的数据直通到输出端
			_trigger.UpdateOutput();
		}
	}

	ReadAndSendFrame();
}
