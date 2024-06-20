#pragma once
#include<ffmpeg-wrapper/wrapper/AVPacketWrapper.h>

namespace video
{
	class IPacketConsumer
	{
	public:
		virtual ~IPacketConsumer() {}

	public:
		virtual void SendPacket(AVPacketWrapper *packet) = 0;
	};
}