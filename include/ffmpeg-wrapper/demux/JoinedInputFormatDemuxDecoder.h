#pragma once
#include <base/MutexHandleWrapper.h>
#include <base/delegate/Delegate.h>
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
		int _original_video_stream_index = -1;
		AVStreamInfoCollection _audio_stream_infos;
		std::shared_ptr<IDecoderPipe> _audio_decode_pipe;
		int _original_audio_stream_index = -1;
		std::shared_ptr<InfinitePacketPipe> _infinite_packet_pipe{new InfinitePacketPipe{}};
		base::List<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> _video_frame_consumer_list;
		base::List<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> _audio_frame_consumer_list;

		base::Delegate<std::shared_ptr<InputFormat> &> _need_input_format_event;

		void InitializeVideoDecoderPipe();
		void InitializeAudioDecoderPipe();
		void GetAndOpenNewInputFormatIfCurrentIsNull();

	public:
		void PumpDataToConsumers(std::shared_ptr<base::CancellationToken> cancel_pump) override;

		/// @brief 本对象需要输入格式时就会触发此回调，传入一个 std::shared_ptr<InputFormat> &
		/// 这是引用，也就意味着可以被回调函数修改。回调函数可以将一个输入格式赋值给传进去的
		/// std::shared_ptr<InputFormat> & 参数，函数返回后，本对象就能获取到这个输入格式。
		///
		/// @return
		base::IEvent<std::shared_ptr<InputFormat> &> &NeedInputFormatEvent()
		{
			return _need_input_format_event;
		}

		void AddVideoFrameConsumer(std::shared_ptr<base::IConsumer<AVFrameWrapper>> consumer);
		void AddAudioFrameConsumer(std::shared_ptr<base::IConsumer<AVFrameWrapper>> consumer);
	};
}
