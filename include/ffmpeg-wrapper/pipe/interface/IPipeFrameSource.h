#pragma once
#include<base/container/List.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>

namespace video
{
	class IPipeFrameSource
	{
	public:
		virtual ~IPipeFrameSource() = default;

		virtual base::List<shared_ptr<IFrameConsumer>> &FrameConsumerList() = 0;

		/// <summary>
		///		向每个消费者送入帧。
		/// </summary>
		void SendFrameToEachConsumer(AVFrameWrapper *frame);
	};
}