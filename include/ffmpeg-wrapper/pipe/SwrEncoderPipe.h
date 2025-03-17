#pragma once
#include <ffmpeg-wrapper/output-format/OutputFormat.h>
#include <ffmpeg-wrapper/pch.h>
#include <ffmpeg-wrapper/pipe/SwrPipe.h>
#include <string>


namespace video
{
	class SwrEncoderPipe :
		public base::IConsumer<AVFrameWrapper>
	{
	private:
		std::shared_ptr<base::IConsumer<AVFrameWrapper>> _encoder_pipe;
		std::shared_ptr<SwrPipe> _swr_pipe;

	public:
		/// @brief 构造函数
		/// @param codec_name 编码器名称。
		/// @param infos 期望编码输出成什么样的音频流。
		/// @note 如果输入的音频帧与期望不符会重采样。
		/// 输入帧的参数会变化也没关系，重采样器会重新构造。
		///
		/// @param output_format 编码后要将包写入的封装。
		SwrEncoderPipe(std::string codec_name,
					   IAudioStreamInfoCollection const &infos,
					   std::shared_ptr<OutputFormat> output_format);

		/// @brief 送入帧。会先经过重采样管道然后编码，最后写入封装。
		/// @param frame
		void SendData(AVFrameWrapper &frame) override;

		void Flush() override;
	};
} // namespace video
