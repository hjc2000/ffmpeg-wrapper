#include "EncoderPipeFactory.h"
#include <ffmpeg-wrapper/pipe/EncoderPipe.h>

using namespace video;

std::shared_ptr<EncoderPipeFactory> video::EncoderPipeFactory::Instance()
{
	static std::shared_ptr<EncoderPipeFactory> o{new EncoderPipeFactory{}};
	return o;
}

std::shared_ptr<base::IConsumer<AVFrameWrapper>> video::EncoderPipeFactory::CreateEncoderPipe(
	std::string codec_name,
	IVideoStreamInfoCollection const &in_stream_infos,
	std::shared_ptr<OutputFormat> output_format,
	int64_t out_bit_rate_in_bps)
{
	return std::shared_ptr<base::IConsumer<AVFrameWrapper>>{
		new EncoderPipe{
			codec_name,
			in_stream_infos,
			output_format,
			out_bit_rate_in_bps,
		},
	};
}

std::shared_ptr<base::IConsumer<AVFrameWrapper>> video::EncoderPipeFactory::CreateEncoderPipe(
	std::string codec_name,
	IAudioStreamInfoCollection const &in_stream_infos,
	std::shared_ptr<OutputFormat> output_format)
{
	return std::shared_ptr<base::IConsumer<AVFrameWrapper>>{
		new EncoderPipe{
			codec_name,
			in_stream_infos,
			output_format,
		},
	};
}
