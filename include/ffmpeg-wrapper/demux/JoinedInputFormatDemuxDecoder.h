#pragma once
#include <ffmpeg-wrapper/pipe/interface/IPump.h>
#include <functional>
#include <memory>

namespace video
{
	class InputFormat;
	class IFrameConsumer;

	/// <summary>
	///		将输入格式拼接起来，进行解封装、解码。一个输入格式结束后会继续获取
	///		下一个输入格式。最终输出来自不同输入格式解封装、解码后的视频帧、音频帧。
	///
	///		这些视频帧、音频帧的时间戳是连续的，但是因为来自不同文件，分辨率、像素格式、采样率、
	///		采样格式等信息可能会变。编码侧要能够自适应，必要时进行重采样。
	/// </summary>
	class JoinedInputFormatDemuxDecoder : public IPump
	{
	private:
		class Context;
		std::shared_ptr<Context> _context;

		void InitializeVideoDecoderPipe();
		void InitializeAudioDecoderPipe();
		void OpenInputIfNull();

	public:
		JoinedInputFormatDemuxDecoder();

		/// <summary>
		///		当需要输入封装时就会触发此回调。
		///		* 回调函数返回 InputFormat 对象则视频流继续。
		///		* 回调函数返回空指针则结束视频流。
		/// </summary>
		std::function<std::shared_ptr<InputFormat>()> _get_format_callback;

		void Pump(std::shared_ptr<base::CancellationToken> cancel_pump) override;

		void AddVideoFrameConsumer(std::shared_ptr<IFrameConsumer> consumer);
		void AddAudioFrameConsumer(std::shared_ptr<IFrameConsumer> consumer);
	};
}