#pragma once
#include <base/pipe/IPipeSource.h>
#include <ffmpeg-wrapper/info-collection/VideoStreamInfoCollection.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <memory>

namespace video
{
	class SwsPipe;
	class FpsAdjustPipe;

	class SwsFpsPipe
		: public base::IConsumer<AVFrameWrapper>,
		  public base::IPipeSource<AVFrameWrapper>
	{
		std::shared_ptr<SwsPipe> _sws_pipe;
		std::shared_ptr<FpsAdjustPipe> _fps_adjust_pipe;

	public:
		SwsFpsPipe(IVideoStreamInfoCollection const &out_video_stream_infos);
		base::IList<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> &ConsumerList() override;
		void SendData(AVFrameWrapper &frame) override;

		void Flush() override
		{
		}
	};
}