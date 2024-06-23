#pragma once
#include <base/container/List.h>
#include <ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include <memory>

namespace video
{
	class IPipeFrameSource
	{
	public:
		virtual ~IPipeFrameSource() = default;

		virtual base::List<std::shared_ptr<IFrameConsumer>> &FrameConsumerList() = 0;

		/// @brief 向每个消费者送入帧。
		/// @param frame
		void SendFrameToEachConsumer(AVFrameWrapper *frame);
	};
}