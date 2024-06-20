#include"HysteresisBlockingPacketQueue.h"

using namespace video;
using namespace jc;

void HysteresisBlockingPacketQueue::Dispose()
{
	if (_disposed) return;
	_disposed = true;

	_packet_queue.Dispose();
}

void HysteresisBlockingPacketQueue::SendPacket(AVPacketWrapper *packet)
{
	if (!packet)
	{
		_packet_queue.Flush();
		return;
	}

	_packet_queue.Enqueue(*packet);
}

int HysteresisBlockingPacketQueue::ReadPacket(AVPacketWrapper &packet)
{
	try
	{
		packet = _packet_queue.Dequeue();
		return 0;
	}
	catch (std::exception &e)
	{
		return (int)ErrorCode::eof;
	}
}
