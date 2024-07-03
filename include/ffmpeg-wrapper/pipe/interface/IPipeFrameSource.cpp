#include "ffmpeg-wrapper/pipe/interface/IPipeFrameSource.h"
#include <memory>

using namespace video;

void IPipeFrameSource::SendFrameToEachConsumer(AVFrameWrapper *frame)
{
	for (std::shared_ptr<thread::IConsumer<AVFrameWrapper>> &consumer : FrameConsumerList())
	{
		if (consumer)
		{
			if (frame)
			{
				consumer->SendData(*frame);
			}
			else
			{
				consumer->Flush();
			}
		}
	}
}
