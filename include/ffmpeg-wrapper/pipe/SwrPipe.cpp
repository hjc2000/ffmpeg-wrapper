#include "ffmpeg-wrapper/pipe/SwrPipe.h"
#include "SwrPipe.h"
#include <ffmpeg-wrapper/ErrorCode.h>

video::SwrPipe::SwrPipe(IAudioFrameInfoCollection &desired_out_frame_infos)
{
	// 先假设输入流和输出流是一样的，随后反正可以重新构造重采样器
	_in_stream_infos = desired_out_frame_infos;
	_desired_out_frame_infos = desired_out_frame_infos;
	_swr = std::shared_ptr<SwrContextWrapper>{
		new SwrContextWrapper{
			_in_stream_infos,
			_desired_out_frame_infos,
		},
	};
}

void video::SwrPipe::ReadAndSendFrame()
{
	while (1)
	{
		bool result = _swr->TryReadData(_swr_out_frame);
		if (!result)
		{
			return;
		}

		SendDataToEachConsumer(_swr_out_frame);
	}
}

void video::SwrPipe::ReadAndSendFrameWithoutFlushingConsumer()
{
	while (1)
	{
		bool result = _swr->TryReadData(_swr_out_frame);
		if (!result)
		{
			return;
		}

		SendDataToEachConsumer(_swr_out_frame);
	}
}

void video::SwrPipe::ReplaceSwr()
{
	std::cout << CODE_POS_STR << "重新构造 swr" << std::endl;

	// 冲洗旧的重采样器
	_swr->Flush();
	ReadAndSendFrameWithoutFlushingConsumer();

	// 构造新的重采样器
	_swr = std::shared_ptr<SwrContextWrapper>{
		new SwrContextWrapper{
			_in_stream_infos,
			_desired_out_frame_infos,
		},
	};
}

void video::SwrPipe::SendData(AVFrameWrapper &frame)
{
	if (ConsumerList().Count() == 0)
	{
		return;
	}

	ReadAndSendFrame();
	if (frame != _in_stream_infos)
	{
		_in_stream_infos = frame;
		ReplaceSwr();
	}

	_swr->SendData(frame);
	ReadAndSendFrame();
}

void video::SwrPipe::Flush()
{
	if (ConsumerList().Count() == 0)
	{
		return;
	}

	ReadAndSendFrame();
	_swr->Flush();
	ReadAndSendFrame();
	FlushEachConsumer();
}
