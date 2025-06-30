#include "SwsPipe.h"

void video::SwsPipe::ReadAndSendFrame()
{
	// 因为 sws 内没有比较深的队列，它只会储存 1 帧，所以不用在循环里 read_frame
	bool result = _sws_context->TryReadData(_sws_out_frame);
	if (!result)
	{
		return;
	}

	SendDataToEachConsumer(_sws_out_frame);
}

void video::SwsPipe::ReplaceSws()
{
	std::cout << CODE_POS_STR << "重新构造 sws。" << std::endl;
	ReadAndSendFrame();

	_sws_context = std::shared_ptr<SwsContextWrapper>{
		new SwsContextWrapper{
			video::SwsContextWrapper_InVideoFrameInfos{_in_video_frame_infos},
			video::SwsContextWrapper_OutVideoFrameInfos{_desire_out_video_frame_infos},
		},
	};
}

video::SwsPipe::SwsPipe(IVideoFrameInfoCollection const &desire_out_video_frame_infos)
{
	_in_video_frame_infos = desire_out_video_frame_infos;
	_desire_out_video_frame_infos = desire_out_video_frame_infos;

	_sws_context = std::shared_ptr<SwsContextWrapper>{
		new SwsContextWrapper{
			video::SwsContextWrapper_InVideoFrameInfos{_in_video_frame_infos},
			video::SwsContextWrapper_OutVideoFrameInfos{_desire_out_video_frame_infos},
		},
	};
}

void video::SwsPipe::SendData(AVFrameWrapper &frame)
{
	// 非冲洗模式
	if (frame != _in_video_frame_infos)
	{
		_in_video_frame_infos = frame;
		ReplaceSws();
	}

	_sws_context->SendData(frame);
	ReadAndSendFrame();
}

void video::SwsPipe::Flush()
{
	// 冲洗模式
	_sws_context->Flush();
	ReadAndSendFrame();
	FlushEachConsumer();
}
