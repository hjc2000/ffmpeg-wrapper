#include"IPipePacketSource.h"

using namespace video;

void IPipePacketSource::SendPacketToEachConsumer(AVPacketWrapper *packet)
{
	for (shared_ptr<IPacketConsumer> &consumer : PacketConsumerList())
	{
		if (consumer)
		{
			consumer->SendPacket(packet);
		}
	}
}
