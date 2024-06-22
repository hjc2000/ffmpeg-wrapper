#pragma once
#include<memory>
#include<stdint.h>
#include<string>

namespace video
{
	class IEncoderPipeFactory;
	class OutputFormat;
	class SwrEncoderPipe;
	class SwsFpsEncoderPipe;
	class IVideoStreamInfoCollection;
	class IAudioStreamInfoCollection;
	class IFrameConsumer;

	/// <summary>
	///		输入视频帧和音频帧，将会编码，然后封装成 ts。
	/// 
	///		本类能够自适应输入的视频帧、音频帧的参数变化。例如：音频帧的采样格式变了之后能够重新构造
	///		重采样器，从而保持输出端的音频帧格式始终不变。
	/// </summary>
	class SptsEncodeMux
	{
	private:
		class Context;
		std::shared_ptr<Context> _context;

		void InitVideoEncodePipe();
		void InitAudioEncodePipe();
		void WriteHeader();

	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="out_format">输出格式</param>
		/// <param name="video_stream_infos">
		///		期望输出的视频流信息。
		///		- 时间基会强制使用 1/90000，video_stream_infos 中的时间基会被忽略。
		/// </param>
		/// <param name="video_codec_name">输出的视频所使用的编码器名称</param>
		/// <param name="video_out_bitrate_in_bps">
		///		期望的视频输出比特率，传入小于等于 0 的数表示自动。
		/// </param>
		/// <param name="audio_stream_infos">
		///		期望输出的音频流信息。
		///		- 时间基会强制使用 1/90000，audio_stream_infos 中的时间基会被忽略。
		/// </param>
		/// <param name="audio_codec_name">输出音频所使用的编码器名称</param>
		SptsEncodeMux(
			std::shared_ptr<video::IEncoderPipeFactory> factory,
			std::shared_ptr<OutputFormat> out_format,
			// 视频相关参数
			IVideoStreamInfoCollection const &video_stream_infos,
			std::string video_codec_name,
			int64_t video_out_bitrate_in_bps,
			// 音频相关参数
			IAudioStreamInfoCollection const &audio_stream_infos,
			std::string audio_codec_name
		);

		std::shared_ptr<IFrameConsumer> VideoEncodePipe();
		std::shared_ptr<IFrameConsumer> AudioEncodePipe();
	};
}

void test_SptsEncodeMux();
