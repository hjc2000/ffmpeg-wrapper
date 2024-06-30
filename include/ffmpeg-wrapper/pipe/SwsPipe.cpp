#include "SwsPipe.h"

void video::SwsPipe::ReadAndSendFrame()
{
	// 因为 sws 内没有比较深的队列，它只会储存 1 帧，所以不用在循环里 read_frame
	int ret = _sws_context->ReadData(_sws_out_frame);
	switch (ret)
	{
	case 0:
	{
		SendFrameToEachConsumer(&_sws_out_frame);
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

void video::SwsPipe::change_sws()
{
	std::cout << CODE_POS_STR << "重新构造 sws。" << std::endl;
	ReadAndSendFrame();
	_sws_context = std::shared_ptr<SwsContextWrapper>{new SwsContextWrapper{
		_in_video_frame_infos,
		_desire_out_video_frame_infos}};
}

video::SwsPipe::SwsPipe(IVideoFrameInfoCollection const &desire_out_video_frame_infos)
{
	_in_video_frame_infos = desire_out_video_frame_infos;
	_desire_out_video_frame_infos = desire_out_video_frame_infos;
	_sws_context = std::shared_ptr<SwsContextWrapper>{new SwsContextWrapper{
		_in_video_frame_infos,
		_desire_out_video_frame_infos}};
}

void video::SwsPipe::SendData(AVFrameWrapper *frame)
{
	if (!frame)
	{
		// 冲洗模式
		_sws_context->SendData(nullptr);
		ReadAndSendFrame();
		return;
	}

	// 非冲洗模式
	if (*frame != _in_video_frame_infos)
	{
		_in_video_frame_infos = *frame;
		change_sws();
	}

	_sws_context->SendData(frame);
	ReadAndSendFrame();
}
