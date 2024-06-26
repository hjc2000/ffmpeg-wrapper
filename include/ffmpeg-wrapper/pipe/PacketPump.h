#pragma once
#include <atomic>
#include <base/pipe/ISource.h>
#include <base/task/CancellationToken.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <ffmpeg-wrapper/pipe/interface/IPipePacketSource.h>
#include <ffmpeg-wrapper/pipe/interface/IPump.h>
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include <jccpp/IDisposable.h>
#include <memory>

namespace video
{
	/// <summary>
	///		从 base::ISource<AVPacketWrapper> 中读取包，送入 IPacketConsumer
	/// </summary>
	class PacketPump
		: public IPipePacketSource,
		  public IPump,
		  public IDisposable
	{
	private:
		std::atomic_bool _disposed = false;
		std::shared_ptr<base::ISource<AVPacketWrapper>> _packet_source;
		base::List<std::shared_ptr<IPacketConsumer>> _consumer_list;

	public:
		PacketPump(std::shared_ptr<base::ISource<AVPacketWrapper>> packet_source)
		{
			_packet_source = packet_source;
		}

		~PacketPump()
		{
			Dispose();
		}

		void Dispose() override
		{
			if (_disposed)
			{
				return;
			}

			_disposed = true;
		}

		base::List<std::shared_ptr<IPacketConsumer>> &PacketConsumerList() override
		{
			return _consumer_list;
		}

		/// <summary>
		///		每次读取包后，在将包送给消费者之前会回调。
		/// </summary>
		std::function<void(AVPacketWrapper *packet)> _on_before_send_packet_to_consumer;

		void Pump(std::shared_ptr<base::CancellationToken> cancellation_token) override;
	};
}
