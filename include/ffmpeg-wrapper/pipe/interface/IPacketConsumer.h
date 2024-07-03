#pragma once
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include <thread/pipe/IConsumer.h>

namespace video
{
	class IPacketConsumer
	{
	public:
		virtual ~IPacketConsumer() = default;

	public:
		virtual void SendPacket(AVPacketWrapper *packet) = 0;
	};
}
