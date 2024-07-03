#include "HysteresisBlockingPacketQueue.h"

using namespace video;
using namespace jc;

void HysteresisBlockingPacketQueue::Dispose()
{
	if (_disposed)
	{
		return;
	}

	_disposed = true;

	_packet_queue.Dispose();
}

void video::HysteresisBlockingPacketQueue::SendData(AVPacketWrapper &data)
{
	_packet_queue.Enqueue(data);
}

void video::HysteresisBlockingPacketQueue::Flush()
{
	_packet_queue.Flush();
}

int HysteresisBlockingPacketQueue::ReadData(AVPacketWrapper &packet)
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
