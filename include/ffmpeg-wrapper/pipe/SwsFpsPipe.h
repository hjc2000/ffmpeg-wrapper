#pragma once
#include <ffmpeg-wrapper/info-collection/VideoStreamInfoCollection.h>
#include <ffmpeg-wrapper/pipe/interface/IPipeFrameSource.h>
#include <memory>

namespace video
{
	class SwsPipe;
	class FpsAdjustPipe;

	class SwsFpsPipe : public IFrameConsumer,
					   public IPipeFrameSource
	{
		std::shared_ptr<SwsPipe> _sws_pipe;
		std::shared_ptr<FpsAdjustPipe> _fps_adjust_pipe;

	public:
		SwsFpsPipe(IVideoStreamInfoCollection const &out_video_stream_infos);
		base::List<std::shared_ptr<IFrameConsumer>> &FrameConsumerList() override;
		void SendData(AVFrameWrapper *frame) override;
	};
}