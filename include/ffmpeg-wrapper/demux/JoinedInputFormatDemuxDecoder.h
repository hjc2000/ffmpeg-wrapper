#pragma once
#include <base/MutexHandleWrapper.h>
#include <base/pipe/Pump.h>
#include <base/task/CancellationToken.h>
#include <ffmpeg-wrapper/factory/DecoderPipeFactoryManager.h>
#include <ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include <ffmpeg-wrapper/input-format/InputFormat.h>
#include <ffmpeg-wrapper/pipe/InfinitePacketPipe.h>
#include <ffmpeg-wrapper/pipe/ThreadDecoderPipe.h>
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include <functional>
#include <memory>

namespace video
{
	/// @brief 将输入格式拼接起来，进行解封装、解码。一个输入格式结束后会继续获取
	/// 下一个输入格式。最终输出来自不同输入格式解封装、解码后的视频帧、音频帧。
	/// 这些视频帧、音频帧的时间戳是连续的，但是因为来自不同文件，分辨率、像素格式、采样率、
	/// 采样格式等信息可能会变。编码侧要能够自适应，必要时进行重采样。
	class JoinedInputFormatDemuxDecoder
		: base::IPump
	{
	private:
		std::shared_ptr<InputFormat> _current_input_format;
		AVStreamInfoCollection _video_stream_infos;
		std::shared_ptr<IDecoderPipe> _video_decode_pipe;
		int _source_video_stream_index = -1;
		AVStreamInfoCollection _audio_stream_infos;
		std::shared_ptr<IDecoderPipe> _audio_decode_pipe;
		int _source_audio_stream_index = -1;
		std::shared_ptr<InfinitePacketPipe> _infinite_packet_pipe{new InfinitePacketPipe{}};
		base::List<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> _video_frame_consumer_list;
		base::List<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> _audio_frame_consumer_list;

		/// @brief 当需要输入格式时就会触发此回调。
		/// @return 回调函数返回 InputFormat 对象则视频流继续。回调函数返回空指针则结束视频流。
		base::MutexHandleWrapper<std::function<std::shared_ptr<InputFormat>()>> _get_format_callback_wrapper;

		void InitializeVideoDecoderPipe();
		void InitializeAudioDecoderPipe();
		void OpenInputIfNull();

	public:
		void PumpDataToConsumers(std::shared_ptr<base::CancellationToken> cancel_pump) override;

		/// @brief 设置输入格式的源。
		/// @note Immediate 是形容词，意为 “立即的”。通过一个回调函数来获取输入格式，这个函数就
		/// 可以理解为立即的输入格式源。与之相对的是非立即的，即；一个类继承了输入格式源接口，然后
		/// 传入这个对象的实例，这种方式就是非立即的。
		///
		/// @param func 用来获取输入格式的函数。
		/// @note 回调函数返回 InputFormat 对象则视频流继续。回调函数返回空指针则结束视频流。
		void SetImmediateInputFormatSource(std::function<std::shared_ptr<InputFormat>()> func);

		void AddVideoFrameConsumer(std::shared_ptr<base::IConsumer<AVFrameWrapper>> consumer);
		void AddAudioFrameConsumer(std::shared_ptr<base::IConsumer<AVFrameWrapper>> consumer);
	};
}
