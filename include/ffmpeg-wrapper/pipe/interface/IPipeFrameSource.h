#pragma once
#include <base/container/List.h>
#include <base/pipe/IConsumer.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <memory>

namespace video
{
	class IPipeFrameSource
	{
	public:
		virtual ~IPipeFrameSource() = default;

		virtual base::List<std::shared_ptr<base::IConsumer<AVFrameWrapper *>>> &FrameConsumerList() = 0;

		/// @brief 向每个消费者送入帧。
		/// @param frame
		void SendFrameToEachConsumer(AVFrameWrapper *frame);
	};
}