#include "ffmpeg-wrapper/container/HysteresisBlockingFrameQueue.h"
#include<ffmpeg-wrapper/ErrorCode.h>

int video::HysteresisBlockingFrameQueue::ReadFrame(AVFrameWrapper &frame)
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

void video::HysteresisBlockingFrameQueue::SendFrame(AVFrameWrapper *frame)
{
	if (!frame)
	{
		_frame_queue.Flush();
		return;
	}

	_frame_queue.Enqueue(*frame);
}
