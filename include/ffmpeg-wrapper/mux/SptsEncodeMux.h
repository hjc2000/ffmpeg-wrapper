#pragma once
#include <base/container/Queue.h>
#include <base/task/TaskCompletionSignal.h>
#include <ffmpeg-wrapper/factory/EncoderPipeFactoryManager.h>
#include <ffmpeg-wrapper/info-collection/AudioStreamInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/VideoStreamInfoCollection.h>
#include <ffmpeg-wrapper/output-format/OutputFormat.h>
#include <ffmpeg-wrapper/pipe/SwrEncoderPipe.h>
#include <ffmpeg-wrapper/pipe/SwsFpsEncoderPipe.h>
#include <iostream>
#include <memory>
#include <stdint.h>
#include <string>

namespace video
{
	/// @brief 输入视频帧和音频帧，将会编码，然后封装成 ts。
	/// @note 本类能够自适应输入的视频帧、音频帧的参数变化。例如：音频帧的采样格式变了之后能够重新构造
	/// 重采样器，从而保持输出端的音频帧格式始终不变。
	class SptsEncodeMux
	{
	private:
		std::shared_ptr<OutputFormat> _out_format;
		VideoStreamInfoCollection _video_stream_infos;
		std::shared_ptr<SwsFpsEncoderPipe> _video_encoder_pipe;
		std::string _video_codec_name;
		int64_t _video_out_bitrate_in_bps = -1;
		AudioStreamInfoCollection _audio_stream_infos;
		std::shared_ptr<SwrEncoderPipe> _audio_encode_pipe;
		std::string _audio_codec_name;

		void InitVideoEncodePipe();
		void InitAudioEncodePipe();
		void WriteHeader();

	public:
		/// @brief 构造函数
		///
		/// @param out_format 输出格式
		///
		/// @param video_stream_infos 期望输出的视频流信息。
		/// @note 时间基会强制使用 1/90000，video_stream_infos 中的时间基会被忽略。
		///
		/// @param video_codec_name 输出的视频所使用的编码器名称
		/// @param video_out_bitrate_in_bps 期望的视频输出比特率，传入小于等于 0 的数表示自动。
		///
		/// @param audio_stream_infos 期望输出的音频流信息。
		/// @note 时间基会强制使用 1/90000，audio_stream_infos 中的时间基会被忽略。
		///
		/// @param audio_codec_name 输出音频所使用的编码器名称
		SptsEncodeMux(std::shared_ptr<OutputFormat> out_format,
					  // 视频相关参数
					  IVideoStreamInfoCollection const &video_stream_infos,
					  std::string video_codec_name,
					  int64_t video_out_bitrate_in_bps,
					  // 音频相关参数
					  IAudioStreamInfoCollection const &audio_stream_infos,
					  std::string audio_codec_name);

		std::shared_ptr<base::IConsumer<AVFrameWrapper>> VideoEncodePipe();
		std::shared_ptr<base::IConsumer<AVFrameWrapper>> AudioEncodePipe();
	};
}

void test_SptsEncodeMux();
