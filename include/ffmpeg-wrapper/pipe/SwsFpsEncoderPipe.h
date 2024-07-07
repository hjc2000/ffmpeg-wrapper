#pragma once
#include <ffmpeg-wrapper/factory/EncoderPipeFactory.h>
#include <ffmpeg-wrapper/pipe/SwsFpsPipe.h>
#include <memory>

namespace video
{
	class SwsFpsEncoderPipe : public base::IConsumer<AVFrameWrapper>
	{
	private:
		std::shared_ptr<SwsFpsPipe> _sws_fps_pipe;
		std::shared_ptr<base::IConsumer<AVFrameWrapper>> _video_encode_pipe;

	public:
		SwsFpsEncoderPipe(
			std::shared_ptr<EncoderPipeFactory> facroty,
			std::shared_ptr<OutputFormat> out_format,
			IVideoStreamInfoCollection const &video_stream_infos,
			std::string video_codec_name,
			int64_t video_out_bitrate_in_bps);

		void SendData(AVFrameWrapper &frame) override;

		void Flush() override;
	};
}