#include"SwsFpsEncoderPipe.h"

using namespace std;
using namespace video;

video::SwsFpsEncoderPipe::SwsFpsEncoderPipe(
	std::shared_ptr<IEncoderPipeFactory> facroty,
	shared_ptr<OutputFormat> out_format,
	IVideoStreamInfoCollection const &video_stream_infos,
	std::string video_codec_name,
	int64_t video_out_bitrate_in_bps
)
{
	_video_encode_pipe = facroty->CreateEncoderPipe(
		video_codec_name,
		video_stream_infos,
		out_format,
		video_out_bitrate_in_bps
	);
	_sws_fps_pipe = shared_ptr<SwsFpsPipe> { new SwsFpsPipe { video_stream_infos } };
	_sws_fps_pipe->FrameConsumerList().Add(_video_encode_pipe);
}

void video::SwsFpsEncoderPipe::SendFrame(AVFrameWrapper *frame)
{
	_sws_fps_pipe->SendFrame(frame);
}
