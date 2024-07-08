#pragma once
#include <base/container/List.h>
#include <base/pipe/IConsumer.h>
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include <memory>

namespace video
{
	class IPipePacketSource
	{
	public:
		virtual ~IPipePacketSource() = default;

		virtual base::List<std::shared_ptr<base::IConsumer<AVPacketWrapper>>> &PacketConsumerList() = 0;

		/// @brief 向每个消费者送入包
		/// @param packet
		void SendPacketToEachConsumer(AVPacketWrapper *packet);
	};
}