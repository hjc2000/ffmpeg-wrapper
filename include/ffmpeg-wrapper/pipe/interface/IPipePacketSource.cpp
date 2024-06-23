#include "IPipePacketSource.h"
#include <memory>

using namespace video;

void IPipePacketSource::SendPacketToEachConsumer(AVPacketWrapper *packet)
{
	for (std::shared_ptr<IPacketConsumer> &consumer : PacketConsumerList())
	{
		if (consumer)
		{
			consumer->SendPacket(packet);
		}
	}
}
