#include "IPipePacketSource.h"
#include <memory>

using namespace video;

void IPipePacketSource::SendPacketToEachConsumer(AVPacketWrapper *packet)
{
	for (std::shared_ptr<base::IConsumer<AVPacketWrapper>> &consumer : PacketConsumerList())
	{
		if (consumer)
		{
			consumer->SendData(*packet);
		}
	}
}
