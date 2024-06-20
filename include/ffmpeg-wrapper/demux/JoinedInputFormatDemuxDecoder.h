#pragma once
#include<ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include<ffmpeg-wrapper/input-format/InputFormat.h>
#include<ffmpeg-wrapper/pipe/InfinitePacketPipe.h>
#include<ffmpeg-wrapper/pipe/interface/IPump.h>
#include<ffmpeg-wrapper/pipe/ThreadDecoderPipe.h>
#include<functional>
#include<memory>
#include<base/task/CancellationToken.h>

namespace video
{
	/// <summary>
	///		将输入格式拼接起来，进行解封装、解码。一个输入格式结束后会继续获取
	///		下一个输入格式。最终输出来自不同输入格式解封装、解码后的视频帧、音频帧。
	/// 
	///		这些视频帧、音频帧的时间戳是连续的，但是因为来自不同文件，分辨率、像素格式、采样率、
	///		采样格式等信息可能会变。编码侧要能够自适应，必要时进行重采样。
	/// </summary>
	class JoinedInputFormatDemuxDecoder :public IPump
	{
	private:
		shared_ptr<InputFormat> _current_input_format;
		AVStreamInfoCollection _video_stream_infos;
		shared_ptr<IDecoderPipe> _video_decode_pipe;
		int _source_video_stream_index = -1;
		AVStreamInfoCollection _audio_stream_infos;
		shared_ptr<IDecoderPipe> _audio_decode_pipe;
		int _source_audio_stream_index = -1;
		shared_ptr<InfinitePacketPipe> _infinite_packet_pipe { new InfinitePacketPipe { } };
		base::List<shared_ptr<IFrameConsumer>> _video_frame_consumer_list;
		base::List<shared_ptr<IFrameConsumer>> _audio_frame_consumer_list;

		void InitializeVideoDecoderPipe();
		void InitializeAudioDecoderPipe();
		void OpenInputIfNull();

	public:
		/// <summary>
		///		当需要输入封装时就会触发此回调。
		///		* 回调函数返回 InputFormat 对象则视频流继续。
		///		* 回调函数返回空指针则结束视频流。
		/// </summary>
		std::function<shared_ptr<InputFormat>()> _get_format_callback;

		void Pump(shared_ptr<base::CancellationToken> cancel_pump) override;

		void AddVideoFrameConsumer(shared_ptr<IFrameConsumer> consumer);
		void AddAudioFrameConsumer(shared_ptr<IFrameConsumer> consumer);
	};
}