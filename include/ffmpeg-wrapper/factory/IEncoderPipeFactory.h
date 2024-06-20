#pragma once
#include<ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/output-format/OutputFormat.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>

namespace video
{
	class IEncoderPipeFactory
	{
	public:
		virtual ~IEncoderPipeFactory() = default;

		/// <summary>
		///		构造视频编码管道
		/// </summary>
		/// <param name="codec_name"></param>
		/// <param name="in_stream_infos"></param>
		/// <param name="output_format"></param>
		/// <param name="out_bit_rate_in_bps"></param>
		/// <returns></returns>
		virtual shared_ptr<IFrameConsumer> CreateEncoderPipe(
			std::string codec_name,
			IVideoStreamInfoCollection const &in_stream_infos,
			shared_ptr<OutputFormat> output_format,
			int64_t out_bit_rate_in_bps = -1
		) = 0;

		/// <summary>
		///		构造音频编码管道
		/// </summary>
		/// <param name="codec_name"></param>
		/// <param name="in_stream_infos"></param>
		/// <param name="output_format"></param>
		/// <returns></returns>
		virtual shared_ptr<IFrameConsumer> CreateEncoderPipe(
			std::string codec_name,
			IAudioStreamInfoCollection const &in_stream_infos,
			shared_ptr<OutputFormat> output_format
		) = 0;
	};
}