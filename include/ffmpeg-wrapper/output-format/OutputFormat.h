#pragma once
#include <base/Wrapper.h>
#include <base/container/List.h>
#include <base/delegate/Delegate.h>
#include <base/pipe/IConsumer.h>
#include <ffmpeg-wrapper/base_include.h>
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVProgramWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace video
{
	/// @brief 输出格式的基类。
	class OutputFormat
		: public base::Wrapper<AVFormatContext>,
		  public base::IConsumer<AVPacketWrapper>
	{
	private:
		std::mutex _not_private_methods_lock;
		uint32_t _flush_times = 0;
		base::Delegate<> _all_streams_flushed_event;

		void WriteTrailer();

	public:
		virtual ~OutputFormat() = default;

		/// @brief 所有流都被冲洗后会触发此事件。此事件会被在后台线程中执行，避免死锁。
		/// @return
		base::IEvent<> &AllStreamFlushedEvent()
		{
			return _all_streams_flushed_event;
		}

		/// @brief 以官方格式打印信息。
		/// @param url 仅仅用来显示而已，没有其他任何用途。
		void DumpFormat(char const *url = "");

		/// @brief 检查此输出格式是否需要设置全局头部。
		/// @return
		bool NeedGlobalHeader();

		/// @brief 创建一个新的流。
		/// @note 此流没有任何信息，需要手动填写流的信息，然后才能 WriteHeader。
		/// @return
		AVStreamWrapper CreateNewStream();

		/// @brief 创建流
		/// @note 会用传进来的编码器来设置流的参数。设置的参数中包括时间基和帧率。
		/// 对于音频流，帧率是无用的，但是设置了也没什么不好的影响。
		///
		/// @param codec_ctx 编码器
		/// @return 创建流成功则返回流
		AVStreamWrapper CreateNewStream(std::shared_ptr<AVCodecContextWrapper> codec_ctx);

		/// @brief 写入头部信息。
		/// @param dic
		void WriteHeader(AVDictionary **dic = nullptr);

		/// @brief 送入包。
		/// @param packet
		void SendData(AVPacketWrapper &packet) override;

		/// @brief 冲洗
		/// @note 会对冲洗进行计数，只有当冲洗次数等于流的数量的那一刻才会真正去冲洗底层的 ffmpeg
		/// 对象。这是为了适应管道。将本对象连接到多个管道中时，本对象会被冲洗多次，只有每个管道都
		/// 冲洗了本对象，才算写入格式完成了，这时候才能冲洗底层的 ffmpeg 对象。
		/// @note 冲洗后，会自动调用 WriteTrailer 方法。
		void Flush() override;
	};
}
