#include "SwsFpsEncoderPipe.h"

video::SwsFpsEncoderPipe::SwsFpsEncoderPipe(std::shared_ptr<OutputFormat> out_format,
											IVideoStreamInfoCollection const &video_stream_infos,
											std::string video_codec_name,
											int64_t video_out_bitrate_in_bps)
{
	_video_encode_pipe = video::EncoderPipeFactoryManager::Instance().Factory()->CreateEncoderPipe(video_codec_name,
																								   video_stream_infos,
																								   out_format,
																								   video_out_bitrate_in_bps);
	_sws_fps_pipe = std::shared_ptr<SwsFpsPipe>{new SwsFpsPipe{video_stream_infos}};
	_sws_fps_pipe->ConsumerList().Add(_video_encode_pipe);
}

void video::SwsFpsEncoderPipe::SendData(AVFrameWrapper &frame)
{
	_sws_fps_pipe->SendData(frame);
}

void video::SwsFpsEncoderPipe::Flush()
{
	std::cout << CODE_POS_STR << "SwsFpsEncoderPipe 被冲洗" << std::endl;
	_sws_fps_pipe->Flush();
}
