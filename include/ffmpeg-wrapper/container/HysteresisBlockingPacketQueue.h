#pragma once
#include <base/container/HysteresisBlockingQueue.h>
#include <base/pipe/ISource.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>

namespace video
{
	/// <summary>
	///		包队列。内部使用带有滞回特性的 HysteresisBlockingQueue
	/// </summary>
	class HysteresisBlockingPacketQueue
		: public base::IConsumer<AVPacketWrapper>,
		  public base::ISource<AVPacketWrapper>,
		  public base::IDisposable
	{
	private:
		base::HysteresisBlockingQueue<AVPacketWrapper> _packet_queue{10};
		std::atomic_bool _disposed = false;

	public:
		/// <summary>
		///		清空队列，取消所有阻塞
		/// </summary>
		void Dispose() override;

		void SendData(AVPacketWrapper &data) override;
		void Flush() override;

		/// <summary>
		///		读取包。
		///		队列被冲洗后，读取完所有包后会遇到 ErrorCode::eof
		/// </summary>
		/// <param name="packet"></param>
		/// <returns></returns>
		int ReadData(AVPacketWrapper &packet);
	};
}
