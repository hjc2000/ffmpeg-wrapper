#pragma once
#include <ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include <ffmpeg-wrapper/output-format/OutputFormat.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <memory>
#include <thread/pipe/IConsumer.h>

namespace video
{
	/// @brief 基于 ffmpeg 的编码管道工厂。
	/// @note 这里创建出来的编码管道的编码器是 ffmpeg 的编码器的包装。
	/// @note 如果移植到特殊平台，需要硬件编码加速，可以自己实现一个编码器工厂，
	/// 继承 video::IEncoderPipeFactory 接口，在这里提供硬件加速的编码器。
	class EncoderPipeFactory
	{
	private:
		EncoderPipeFactory() = default;

	public:
		/// @brief 返回单例
		/// @note 这里返回共享指针而不是引用，是因为工厂实例是作为依赖注入到需要它的类中，
		/// 并且在那个类的生命周期内都要存在。
		///
		/// @return
		static std::shared_ptr<EncoderPipeFactory> Instance();

		/// @brief 构造视频编码管道
		/// @param codec_name
		/// @param in_stream_infos
		/// @param output_format
		/// @param out_bit_rate_in_bps
		/// @return
		std::shared_ptr<thread::IConsumer<AVFrameWrapper>> CreateEncoderPipe(
			std::string codec_name,
			IVideoStreamInfoCollection const &in_stream_infos,
			std::shared_ptr<OutputFormat> output_format,
			int64_t out_bit_rate_in_bps = -1);

		/// @brief 构造音频编码管道
		/// @param codec_name
		/// @param in_stream_infos
		/// @param output_format
		/// @return
		std::shared_ptr<thread::IConsumer<AVFrameWrapper>> CreateEncoderPipe(
			std::string codec_name,
			IAudioStreamInfoCollection const &in_stream_infos,
			std::shared_ptr<OutputFormat> output_format);
	};
}
