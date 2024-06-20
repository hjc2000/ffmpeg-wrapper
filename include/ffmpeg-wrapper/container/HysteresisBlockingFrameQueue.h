#pragma once
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameSource.h>
#include<jccpp/IDisposable.h>
#include<jccpp/container/HysteresisBlockingQueue.h>

namespace video
{
	class HysteresisBlockingFrameQueue :
		public IFrameConsumer,
		public IFrameSource,
		public IDisposable
	{
		std::atomic_bool _disposed = false;
		jc::HysteresisBlockingQueue<AVFrameWrapper> _frame_queue{ 10 };

	public:
		void Dispose()
		{
			if (_disposed) return;
			_disposed = true;

			_frame_queue.Dispose();
		}

		int ReadFrame(AVFrameWrapper &frame) override;
		void SendFrame(AVFrameWrapper *frame) override;
	};
}