#include "ffmpeg-wrapper/container/HysteresisBlockingFrameQueue.h"
#include <ffmpeg-wrapper/ErrorCode.h>

int video::HysteresisBlockingFrameQueue::ReadData(AVFrameWrapper &frame)
{
	try
	{
		frame = _frame_queue.Dequeue();
		return 0;
	}
	catch (std::runtime_error &e)
	{
		return (int)ErrorCode::eof;
	}
}

void video::HysteresisBlockingFrameQueue::SendData(AVFrameWrapper &frame)
{
	_frame_queue.Enqueue(frame);
}
