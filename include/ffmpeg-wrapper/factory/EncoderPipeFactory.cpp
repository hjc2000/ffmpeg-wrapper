#include "EncoderPipeFactory.h"
#include<EncoderPipe.h>

using namespace video;

std::shared_ptr<EncoderPipeFactory> video::EncoderPipeFactory::Instance()
{
	static std::shared_ptr<EncoderPipeFactory> o { new  EncoderPipeFactory { } };
	return o;
}

shared_ptr<IFrameConsumer> video::EncoderPipeFactory::CreateEncoderPipe(
	std::string codec_name,
	IVideoStreamInfoCollection const &in_stream_infos,
	shared_ptr<OutputFormat> output_format,
	int64_t out_bit_rate_in_bps
)
{
	return shared_ptr<IFrameConsumer>{new EncoderPipe {
		codec_name,
		in_stream_infos,
		output_format,
		out_bit_rate_in_bps,
	}};
}

shared_ptr<IFrameConsumer> video::EncoderPipeFactory::CreateEncoderPipe(
	std::string codec_name,
	IAudioStreamInfoCollection const &in_stream_infos,
	shared_ptr<OutputFormat> output_format
)
{
	return shared_ptr<IFrameConsumer>{new EncoderPipe {
		codec_name,
		in_stream_infos,
		output_format
	}};
}
