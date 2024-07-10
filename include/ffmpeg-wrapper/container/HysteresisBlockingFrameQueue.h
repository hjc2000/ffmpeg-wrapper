#pragma once
#include <base/IDisposable.h>
#include <base/container/HysteresisBlockingQueue.h>
#include <base/pipe/IConsumer.h>
#include <base/pipe/ISource.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>

namespace video
{
	class HysteresisBlockingFrameQueue
		: public base::IConsumer<AVFrameWrapper>,
		  public base::ISource<AVFrameWrapper>,
		  public base::IDisposable
	{
	private:
		std::atomic_bool _disposed = false;
		base::HysteresisBlockingQueue<AVFrameWrapper> _frame_queue{10};

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
