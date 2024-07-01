#pragma once
#include <atomic>
#include <base/pipe/IConsumer.h>
#include <base/pipe/ISource.h>
#include <ffmpeg-wrapper/info-collection/AudioStreamInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/VideoStreamInfoCollection.h>
#include <ffmpeg-wrapper/output-format/OutputFormat.h>
#include <ffmpeg-wrapper/wrapper/AVCodecContextWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include <jccpp/IDisposable.h>

namespace video
{
	/// @brief 连接向 OutputFormat 的编码管道。构造本类对象，在构造函数中传入 OutputFormat 对象，
	/// 会在那个 OutputFormat 中创建新的流并与本 EncoderPipe 对象绑定。
	/// 向本对象送入帧后会编码，然后写入 OutputFormat。
	class EncoderPipe
		: public base::IConsumer<AVFrameWrapper *>
	{
	private:
		std::shared_ptr<AVCodecContextWrapper> _encoder_ctx;
		AVStreamWrapper _new_stream;
		std::shared_ptr<OutputFormat> _output_format;

		/// @brief 读取包并写入封装
		void ReadAndSendPacketToOutputFormat();

	public:
		/// @brief 根据视频流信息构造视频编码器管道
		/// @param codec_name 编码器名称
		/// @param in_stream_infos 编码器输入流的信息
		/// @param output_format 编码后的包要写入的格式
		/// @param out_bit_rate_in_bps 想要输出的比特率。设置越大质量越高。设置为小于等于 0 表示自动。
		EncoderPipe(
			std::string codec_name,
			IVideoStreamInfoCollection const &in_stream_infos,
			std::shared_ptr<OutputFormat> output_format,
			int64_t out_bit_rate_in_bps = -1);

		/// @brief 根据音频流信息构造音频编码器管道。
		/// @param codec_name 编码器名称。
		/// @param in_stream_infos 编码器输入流信息。
		/// @param output_format 编码后的包要写入的封装。
		EncoderPipe(
			std::string codec_name,
			IAudioStreamInfoCollection const &in_stream_infos,
			std::shared_ptr<OutputFormat> output_format);

		/// @brief 送入帧进行编码。
		/// @note 编码后的包会被写入构造函数中传进来的输出格式中。
		///
		/// @param frame
		void SendData(AVFrameWrapper *frame) override;

		void Flush() override
		{
		}
	};
}
