#pragma once
#include <base/pipe/PipeBlockingQueue.h>
#include <base/string/define.h>
#include <base/task/TaskCompletionSignal.h>
#include <ffmpeg-wrapper/factory/DecoderPipeFactoryManager.h>
#include <ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include <ffmpeg-wrapper/pipe/interface/IDecoderPipe.h>
#include <memory>

namespace video
{
	class ThreadDecoderPipe final :
		public IDecoderPipe
	{
	private:
		std::shared_ptr<IDecoderPipe> _decoder_pipe;
		base::PipeBlockingQueue<AVPacketWrapper> _packet_queue{};
		base::task::TaskCompletionSignal _decode_thread_exit{true};
		std::atomic_bool _do_not_flush_consumer = false;
		std::atomic_bool _disposed = false;

		void InitDecodeThread();
		void DecodeThreadFunc();

	public:
		/// @brief 将根据 stream 的信息构造一个解码管道。
		/// @param stream
		ThreadDecoderPipe(AVStreamInfoCollection stream);
		~ThreadDecoderPipe();
		void Dispose() override;

	public:
		/// @brief 送入包
		/// @note 将包送入队列后就会立即返回，队列满了才会受到阻塞。
		/// 另一个线程会负责从队列中取出包进行解码。
		///
		/// @param packet
		void SendData(AVPacketWrapper &packet) override;

		/// @brief 冲洗消费者
		void Flush() override;

		/// @brief 冲洗解码器，但是不冲洗消费者。
		/// @note 当需要把本解码器管道撤掉，换另一个解码器管道时就要调用本方法。
		/// 这样既可以取出残留在解码器中的帧，又不会把下级的消费者也一起冲洗了。
		virtual void FlushDecoderButNotFlushConsumers() override;

		AVRational TimeBase() const override;
		void SetTimeBase(AVRational value) override;

		AVSampleFormat SampleFormat() const override;
		void SetSampleFormat(AVSampleFormat value) override;

		int SampleRate() const override;
		void SetSampleRate(int value) override;

		AVChannelLayout ChannelLayout() const override;
		void SetChannelLayout(AVChannelLayout value) override;

		int Width() const override;
		void SetWidth(int value) override;

		int Height() const override;
		void SetHeight(int value) override;

		AVPixelFormat PixelFormat() const override;
		void SetPixelFormat(AVPixelFormat value) override;

		AVRational FrameRate() const override;
		void SetFrameRate(AVRational value) override;

		base::IList<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> &ConsumerList() override
		{
			return _decoder_pipe->ConsumerList();
		}
	};
} // namespace video
