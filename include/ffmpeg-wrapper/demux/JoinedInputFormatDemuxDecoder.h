#pragma once
#include <ffmpeg-wrapper/pipe/interface/IPump.h>
#include <functional>
#include <memory>

namespace video
{
	class InputFormat;
	class IFrameConsumer;

	/// @brief 将输入格式拼接起来，进行解封装、解码。一个输入格式结束后会继续获取
	/// 下一个输入格式。最终输出来自不同输入格式解封装、解码后的视频帧、音频帧。
	/// 这些视频帧、音频帧的时间戳是连续的，但是因为来自不同文件，分辨率、像素格式、采样率、
	/// 采样格式等信息可能会变。编码侧要能够自适应，必要时进行重采样。
	class JoinedInputFormatDemuxDecoder : public IPump
	{
	private:
		class Context;
		std::shared_ptr<Context> _context;

		/// @brief 当需要输入格式时就会触发此回调。
		/// @return 回调函数返回 InputFormat 对象则视频流继续。回调函数返回空指针则结束视频流。
		std::function<std::shared_ptr<InputFormat>()> _get_format_callback;

		void InitializeVideoDecoderPipe();
		void InitializeAudioDecoderPipe();
		void OpenInputIfNull();

	public:
		JoinedInputFormatDemuxDecoder();

		void Pump(std::shared_ptr<base::CancellationToken> cancel_pump) override;

		/// @brief 设置输入格式的源。
		/// @note Immediate 是形容词，意为 “立即的”。通过一个回调函数来获取输入格式，这个函数就
		/// 可以理解为立即的输入格式源。与之相对的是非立即的，即；一个类继承了输入格式源接口，然后
		/// 传入这个对象的实例，这种方式就是非立即的。
		///
		/// @param func 用来获取输入格式的函数。
		/// @note 回调函数返回 InputFormat 对象则视频流继续。回调函数返回空指针则结束视频流。
		void SetImmediateInputFormatSource(std::function<std::shared_ptr<InputFormat>()> func)
		{
			_get_format_callback = func;
		}

		void AddVideoFrameConsumer(std::shared_ptr<IFrameConsumer> consumer);
		void AddAudioFrameConsumer(std::shared_ptr<IFrameConsumer> consumer);
	};
}
