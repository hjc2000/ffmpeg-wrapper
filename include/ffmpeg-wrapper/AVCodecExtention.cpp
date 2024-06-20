#include "ffmpeg-wrapper/AVCodecExtention.h"

const AVCodec *video::AVCodecExtention::find_encoder_by_id(AVCodecID id)
{
	return avcodec_find_encoder(id);
}

const AVCodec *video::AVCodecExtention::find_encoder_by_name(const char *name)
{
	return ::avcodec_find_encoder_by_name(name);
}

const AVCodec *video::AVCodecExtention::find_decoder_by_id(AVCodecID id)
{
	return ::avcodec_find_decoder(id);
}

const AVCodec *video::AVCodecExtention::find_decoder_by_name(const char *name)
{
	return avcodec_find_decoder_by_name(name);
}
