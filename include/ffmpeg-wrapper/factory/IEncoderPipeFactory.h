#pragma once
#include <base/pipe/IConsumer.h>
#include <ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include <ffmpeg-wrapper/output-format/OutputFormat.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <memory>

namespace video
{
	class IEncoderPipeFactory
	{
	public:
		virtual ~IEncoderPipeFactory() = default;

		/// @brief 构造视频编码管道
		/// @param codec_name
		/// @param in_stream_infos
		/// @param output_format
		/// @param out_bit_rate_in_bps
		/// @return
		virtual std::shared_ptr<base::IConsumer<AVFrameWrapper>> CreateEncoderPipe(
			std::string codec_name,
			IVideoStreamInfoCollection const &in_stream_infos,
			std::shared_ptr<OutputFormat> output_format,
			int64_t out_bit_rate_in_bps = -1) = 0;

		/// @brief 构造音频编码管道
		/// @param codec_name
		/// @param in_stream_infos
		/// @param output_format
		/// @return
		virtual std::shared_ptr<base::IConsumer<AVFrameWrapper>> CreateEncoderPipe(
			std::string codec_name,
			IAudioStreamInfoCollection const &in_stream_infos,
			std::shared_ptr<OutputFormat> output_format) = 0;
	};
} // namespace video
