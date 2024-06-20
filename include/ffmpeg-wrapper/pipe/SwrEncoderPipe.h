#pragma once
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/factory/IEncoderPipeFactory.h>
#include<ffmpeg-wrapper/output-format/OutputFormat.h>
#include<ffmpeg-wrapper/pipe/SwrPipe.h>
#include<string>

namespace video
{
	/// <summary>
	///		入口处有重采样器管道的编码器管道。能够接收采样格式，采样率等参数动态变化的音频帧，
	///		输出统一参数的音频包。
	/// </summary>
	class SwrEncoderPipe :public IFrameConsumer
	{
	private:
		shared_ptr<IFrameConsumer> _encoder_pipe;
		shared_ptr<SwrPipe> _swr_pipe;

	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="codec_name">编码器名称。全部小写。</param>
		/// <param name="infos">
		///		期望编码输出成什么样的音频流。如果输入的音频帧与期望不符会重采样。
		///		输入帧的参数会变化也没关系，重采样器会重新构造。
		/// </param>
		/// <param name="output_format">编码后要将包写入的封装。</param>
		SwrEncoderPipe(
			std::shared_ptr<IEncoderPipeFactory> facroty,
			std::string codec_name,
			IAudioStreamInfoCollection &infos,
			shared_ptr<OutputFormat> output_format
		);

		/// <summary>
		///		送入帧。会先经过重采样管道然后编码，最后写入封装。
		/// </summary>
		/// <param name="frame"></param>
		void SendFrame(AVFrameWrapper *frame) override;
	};
}