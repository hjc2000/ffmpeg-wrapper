#include "AVCodecExtension.h"

AVCodec const *video::AVCodecExtension::FindEncoderById(AVCodecID id)
{
	return avcodec_find_encoder(id);
}

AVCodec const *video::AVCodecExtension::FindEncoderByName(char const *name)
{
	return ::avcodec_find_encoder_by_name(name);
}

AVCodec const *video::AVCodecExtension::FindDecoderById(AVCodecID id)
{
	return ::avcodec_find_decoder(id);
}

AVCodec const *video::AVCodecExtension::FindDecoderByName(char const *name)
{
	return avcodec_find_decoder_by_name(name);
}
