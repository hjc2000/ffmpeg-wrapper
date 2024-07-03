#pragma once
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <jccpp/IDisposable.h>
#include <jccpp/container/HysteresisBlockingQueue.h>
#include <thread/pipe/IConsumer.h>
#include <thread/pipe/ISource.h>

namespace video
{
	class HysteresisBlockingFrameQueue
		: public thread::IConsumer<AVFrameWrapper>,
		  public thread::ISource<AVFrameWrapper>,
		  public IDisposable
	{
	private:
		std::atomic_bool _disposed = false;
		jc::HysteresisBlockingQueue<AVFrameWrapper> _frame_queue{10};

	public:
		void Dispose()
		{
			if (_disposed)
			{
				return;
			}

			_disposed = true;

			_frame_queue.Dispose();
		}

		int ReadData(AVFrameWrapper &frame) override;
		void SendData(AVFrameWrapper &frame) override;

		void Flush() override
		{
			_frame_queue.Flush();
		}
	};
}