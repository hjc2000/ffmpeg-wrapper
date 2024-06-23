#pragma once
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>

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
