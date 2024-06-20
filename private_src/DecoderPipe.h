#pragma once
#include<atomic>
#include<base/container/List.h>
#include<ffmpeg-wrapper/AVToString.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IDecoderPipe.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<ffmpeg-wrapper/wrapper/AVCodecContextWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include<jccpp/TaskCompletionSignal.h>
#include<vector>

namespace video
{
	class DecoderPipe final :public IDecoderPipe
	{
		AVStreamInfoCollection _stream_infos;
		std::atomic_bool _disposed = false;
		shared_ptr<AVCodecContextWrapper> _decoder;
		AVFrameWrapper _decoder_out_frame;
		base::List<shared_ptr<IFrameConsumer>> _consumer_list;

		void read_and_send_frame();

	public:
		DecoderPipe(AVStreamInfoCollection stream);
		~DecoderPipe();

		/// <summary>
		///		释放后 read_and_send_frame 函数内如果正在进行循环向消费者送入帧，
		///		则会在当前循环完成后不再执行下一轮循环。
		/// </summary>
		void Dispose() override;

		base::List<shared_ptr<IFrameConsumer>> &FrameConsumerList() override
		{
			return _consumer_list;
		}

		/// <summary>
		///		送入包。会解码，然后将帧送给消费者。
		///		如果没有任何消费者，本函数会直接返回，不会真正去解码，这样可以节省性能。
		///		如果送入的包的流索引和初始化本对象时的流索引不匹配，会直接返回，不会执行解码。
		/// </summary>
		/// <param name="packet"></param>
		void SendPacket(AVPacketWrapper *packet) override;
		void FlushDecoderButNotFlushConsumers() override;

		#pragma region 通过 IAudioStreamInfoCollection 继承
		AVRational TimeBase() const override;
		void SetTimeBase(AVRational value) override;
		AVSampleFormat SampleFormat() const override;
		void SetSampleFormat(AVSampleFormat value) override;
		int SampleRate() const override;
		void SetSampleRate(int value) override;
		AVChannelLayout ChannelLayout() const override;
		void SetChannelLayout(AVChannelLayout value) override;
		#pragma endregion

		#pragma region 通过 IVideoStreamInfoCollection 继承
		int Width() const override;
		void SetWidth(int value) override;
		int Height() const override;
		void SetHeight(int value) override;
		AVPixelFormat PixelFormat() const override;
		void SetPixelFormat(AVPixelFormat value) override;
		AVRational FrameRate() const override;
		void SetFrameRate(AVRational value) override;
		#pragma endregion

	};
}