#pragma once
#include<atomic>
#include<ffmpeg-wrapper/info-collection/AudioStreamInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/VideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/output-format/OutputFormat.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketSource.h>
#include<ffmpeg-wrapper/wrapper/AVCodecContextWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include<jccpp/IDisposable.h>

namespace video
{
	/// <summary>
	///		连接向 OutputFormat 的编码管道。构造本类对象，在构造函数中传入 OutputFormat 对象，
	///		会在那个 OutputFormat 中创建新的流并与本 EncoderPipe 对象绑定。
	/// 
	///		向本对象送入帧后会编码，然后写入 OutputFormat。
	/// </summary>
	class EncoderPipe :public IFrameConsumer
	{
	private:
		shared_ptr<AVCodecContextWrapper> _encoder_ctx;
		AVStreamWrapper _new_stream;
		shared_ptr<OutputFormat> _output_format;

		/// <summary>
		///		读取包并写入封装
		/// </summary>
		void ReadAndSendPacketToOutputFormat();

	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="codec_name">编码器名称</param>
		/// <param name="in_stream_infos">编码器输入流的信息</param>
		/// <param name="output_format">编码后的包要写入的格式</param>
		/// <param name="out_bit_rate_in_bps">
		///		想要输出的比特率。设置越大质量越高。设置为小于等于 0 表示自动。
		/// </param>
		EncoderPipe(
			std::string codec_name,
			IVideoStreamInfoCollection const &in_stream_infos,
			shared_ptr<OutputFormat> output_format,
			int64_t out_bit_rate_in_bps = -1
		);

		/// <summary>
		///		
		/// </summary>
		/// <param name="codec_name">编码器名称。</param>
		/// <param name="in_stream_infos">编码器输入流信息。</param>
		/// <param name="output_format">编码后的包要写入的封装。</param>
		EncoderPipe(
			std::string codec_name,
			IAudioStreamInfoCollection const &in_stream_infos,
			shared_ptr<OutputFormat> output_format
		);

		/// <summary>
		///		送入帧进行编码，然后将包送给消费者。
		/// </summary>
		/// <param name="frame"></param>
		void SendFrame(AVFrameWrapper *frame) override;
	};
}
