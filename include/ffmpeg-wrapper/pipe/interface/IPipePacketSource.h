#pragma once
#include<base/container/List.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<memory>

namespace video
{
	class IPipePacketSource
	{
	public:
		virtual ~IPipePacketSource() = default;

		virtual base::List<std::shared_ptr<IPacketConsumer>> &PacketConsumerList() = 0;

		/// <summary>
		///		向每个消费者送入包
		/// </summary>
		/// <param name="packet"></param>
		void SendPacketToEachConsumer(AVPacketWrapper *packet);
	};
}