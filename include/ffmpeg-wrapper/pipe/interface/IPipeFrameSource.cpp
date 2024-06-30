#include "ffmpeg-wrapper/pipe/interface/IPipeFrameSource.h"
#include <memory>

using namespace video;

void IPipeFrameSource::SendFrameToEachConsumer(AVFrameWrapper *frame)
{
	for (std::shared_ptr<base::IConsumer<AVFrameWrapper *>> &consumer : FrameConsumerList())
	{
		if (consumer)
		{
			consumer->SendData(frame);
		}
	}
}
