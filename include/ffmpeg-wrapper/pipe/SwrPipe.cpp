#include "ffmpeg-wrapper/pipe/SwrPipe.h"
#include<ffmpeg-wrapper/ErrorCode.h>

video::SwrPipe::SwrPipe(IAudioFrameInfoCollection &desired_out_frame_infos)
{
	// 先假设输入流和输出流是一样的，随后反正可以重新构造重采样器
	_in_stream_infos = desired_out_frame_infos;
	_desired_out_frame_infos = desired_out_frame_infos;
	_swr = shared_ptr<SwrContextWrapper>{ new SwrContextWrapper{_in_stream_infos, _desired_out_frame_infos} };
}

void video::SwrPipe::read_and_send_frame()
{
	while (1)
	{
		int ret = _swr->ReadFrame(_swr_out_frame);
		switch (ret)
		{
		case 0:
			{
				SendFrameToEachConsumer(&_swr_out_frame);

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
		default:
			{
				throw std::runtime_error{ ToString((ErrorCode)ret) };
			}
		}
	}
}

void video::SwrPipe::read_and_send_frame_without_flushing_consumer()
{
	while (1)
	{
		int ret = _swr->ReadFrame(_swr_out_frame);
		switch (ret)
		{
		case 0:
			{
				SendFrameToEachConsumer(&_swr_out_frame);

				// 下轮循环继续读取
				break;
			}
		case (int)ErrorCode::output_is_temporarily_unavailable:
			{
				return;
			}
		case (int)ErrorCode::eof:
			{
				return;
			}
		default:
			{
				throw std::runtime_error{ ToString((ErrorCode)ret) };
			}
		}
	}
}

void video::SwrPipe::change_swr()
{
	cout << CODE_POS_STR << "重新构造 swr" << endl;

	// 冲洗旧的重采样器
	_swr->SendFrame(nullptr);
	read_and_send_frame_without_flushing_consumer();

	// 构造新的重采样器
	_swr = shared_ptr<SwrContextWrapper>{ new SwrContextWrapper{_in_stream_infos, _desired_out_frame_infos} };
}

void video::SwrPipe::SendFrame(AVFrameWrapper *frame)
{
	if (FrameConsumerList().Count() == 0)
	{
		return;
	}

	read_and_send_frame();
	if (frame && *frame != _in_stream_infos)
	{
		_in_stream_infos = *frame;
		change_swr();
	}

	_swr->SendFrame(frame);
	read_and_send_frame();
}
