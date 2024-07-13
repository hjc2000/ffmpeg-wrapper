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

		void DumpFormat(char const *url = "");

		/// @brief 检查此输出格式是否需要设置全局头部。
		/// @return
		bool NeedGlobalHeader();

		/// <summary>
		///		送入包。
		///		* 送入空指针进行冲洗。会对冲洗进行计数，只有当冲洗次数
		///		  等于流的数量的那一刻才会真正执行冲洗工作。注意，这里只是进行计数，
		///		  不会检查是是真的对每一路流进行冲洗了，还是只是对一路流多次冲洗。
		///		  所以，不要对同一路流进行多次冲洗。
		///		* 冲洗后，会自动调用 WriteTrailer 方法。
		/// </summary>
		/// <param name="packet"></param>
		void SendData(AVPacketWrapper &packet) override;
		void Flush() override;

		void WriteHeader(AVDictionary **dic = nullptr);

		AVStreamWrapper CreateNewStream();

		/// <summary>
		///		创建流，并用传进来的编码器来设置流的参数。设置的参数中包括时间基和帧率。
		///		对于音频流，帧率是无用的，但是设置了也没什么不好的影响。
		/// </summary>
		/// <param name="codec_ctx"></param>
		/// <returns>创建流成功则返回流</returns>
		AVStreamWrapper CreateNewStream(std::shared_ptr<AVCodecContextWrapper> codec_ctx);
	};
}
