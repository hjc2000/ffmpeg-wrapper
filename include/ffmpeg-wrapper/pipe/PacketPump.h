#pragma once
#include<atomic>
#include<base/task/CancellationToken.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketSource.h>
#include<ffmpeg-wrapper/pipe/interface/IPipePacketSource.h>
#include<ffmpeg-wrapper/pipe/interface/IPump.h>
#include<jccpp/IDisposable.h>

namespace video
{
	/// <summary>
	///		从 IPacketSource 中读取包，送入 IPacketConsumer
	/// </summary>
	class PacketPump :
		public IPipePacketSource,
		public IPump,
		public IDisposable
	{
		std::atomic_bool _disposed = false;
		shared_ptr<IPacketSource> _packet_source;
		base::List<shared_ptr<IPacketConsumer>> _consumer_list;

	public:
		PacketPump(shared_ptr<IPacketSource> packet_source)
		{
			_packet_source = packet_source;
		}

		~PacketPump()
		{
			Dispose();
		}

		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;
		}

		base::List<shared_ptr<IPacketConsumer>> &PacketConsumerList() override
		{
			return _consumer_list;
		}

		/// <summary>
		///		每次读取包后，在将包送给消费者之前会回调。
		/// </summary>
		std::function<void(AVPacketWrapper *packet)> _on_before_send_packet_to_consumer;

		void Pump(shared_ptr<base::CancellationToken> cancellation_token) override;
	};
}