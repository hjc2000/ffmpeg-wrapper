#pragma once
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketSource.h>
#include<ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include<jccpp/container/HysteresisBlockingQueue.h>

namespace video
{
	/// <summary>
	///		包队列。内部使用带有滞回特性的 HysteresisBlockingQueue
	/// </summary>
	class HysteresisBlockingPacketQueue :
		public IPacketConsumer,
		public IPacketSource,
		public IDisposable
	{
	private:
		jc::HysteresisBlockingQueue<AVPacketWrapper> _packet_queue { 10 };
		std::atomic_bool _disposed = false;

	public:
		/// <summary>
		///		清空队列，取消所有阻塞
		/// </summary>
		void Dispose() override;

		/// <summary>
		///		向队列送入包
		/// </summary>
		/// <param name="packet">
		///		送入空指针冲洗内部队列。
		///		冲洗后，送入包不会再被阻塞，而是会抛出异常。
		/// </param>
		void SendPacket(AVPacketWrapper *packet);

		/// <summary>
		///		读取包。
		///		队列被冲洗后，读取完所有包后会遇到 ErrorCode::eof
		/// </summary>
		/// <param name="packet"></param>
		/// <returns></returns>
		int ReadPacket(AVPacketWrapper &packet);
	};
}