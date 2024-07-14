#include "ffmpeg-wrapper/pipe/SwrEncoderPipe.h"
#include "SwrEncoderPipe.h"
#include <ffmpeg-wrapper/AVSampleFormatExtention.h>
#include <ffmpeg-wrapper/factory/EncoderPipeFactoryManager.h>

video::SwrEncoderPipe::SwrEncoderPipe(std::string codec_name,
									  IAudioStreamInfoCollection const &infos,
									  shared_ptr<OutputFormat> output_format)
{
	AudioFrameInfoCollection swr_out_frame_infos{
		infos,
		AVSampleFormatExtention::ParseRequiredSampleCount(codec_name),
	};

	_swr_pipe = shared_ptr<SwrPipe>{new SwrPipe{swr_out_frame_infos}};
	_encoder_pipe = video::EncoderPipeFactoryManager::Instance().Factory()->CreateEncoderPipe(codec_name,
																							  infos,
																							  output_format);

	_swr_pipe->ConsumerList().Add(_encoder_pipe);
}

void video::SwrEncoderPipe::SendData(AVFrameWrapper &frame)
{
	_swr_pipe->SendData(frame);
}

void video::SwrEncoderPipe::Flush()
{
	_swr_pipe->Flush();
}
