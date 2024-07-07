#include "SwsFpsPipe.h"
#include <ffmpeg-wrapper/pipe/FpsAdjustPipe.h>
#include <ffmpeg-wrapper/pipe/SwsPipe.h>
#include <memory>

using namespace video;

base::IList<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> &SwsFpsPipe::ConsumerList()
{
	return _fps_adjust_pipe->ConsumerList();
}

video::SwsFpsPipe::SwsFpsPipe(IVideoStreamInfoCollection const &out_video_stream_infos)
{
	_fps_adjust_pipe = std::shared_ptr<FpsAdjustPipe>{
		new FpsAdjustPipe{
			out_video_stream_infos,
			out_video_stream_infos.FrameRate(),
		},
	};
	_sws_pipe = std::shared_ptr<SwsPipe>{new SwsPipe{out_video_stream_infos}};
	_sws_pipe->ConsumerList().Add(_fps_adjust_pipe);
}

void video::SwsFpsPipe::SendData(AVFrameWrapper &frame)
{
	_sws_pipe->SendData(frame);
}
