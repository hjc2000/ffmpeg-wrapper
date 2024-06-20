#include<ffmpeg-wrapper/AVCodecExtention.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/wrapper/AVCodecContextWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>

using namespace video;

AVStreamWrapper::AVStreamWrapper()
{

}

AVStreamWrapper::AVStreamWrapper(AVStream *p)
{
	_wrapped_obj = p;
}

AVStreamWrapper::AVStreamWrapper(AVStreamWrapper const &other)
{
	_wrapped_obj = other._wrapped_obj;
}

AVStreamWrapper &AVStreamWrapper::operator=(AVStreamWrapper const &other)
{
	_wrapped_obj = other._wrapped_obj;
	return *this;
}

AVCodec const *video::AVStreamWrapper::Codec() const
{
	return AVCodecExtention::find_decoder_by_id(_wrapped_obj->codecpar->codec_id);
}

AVCodecParameters &AVStreamWrapper::CodecParams() const
{
	return *_wrapped_obj->codecpar;
}

void video::AVStreamWrapper::SetCodecParams(AVCodecParameters const &params)
{
	avcodec_parameters_copy(_wrapped_obj->codecpar, &params);
}

int video::AVStreamWrapper::SetCodecParams(shared_ptr<AVCodecContextWrapper> codec_ctx)
{
	// 设置时间基
	_wrapped_obj->time_base = (*codec_ctx)->time_base;
	_wrapped_obj->avg_frame_rate = codec_ctx->FrameRate();

	// 将码器的参数设置到本流
	return avcodec_parameters_from_context(_wrapped_obj->codecpar, *codec_ctx);
}

int64_t AVStreamWrapper::Bitrate() const
{
	return _wrapped_obj->codecpar->bit_rate;
}

AVMediaType AVStreamWrapper::MediaType() const
{
	return _wrapped_obj->codecpar->codec_type;
}

int AVStreamWrapper::Index() const
{
	return _wrapped_obj->index;
}

void video::AVStreamWrapper::SetIndex(int value)
{
	_wrapped_obj->index = value;
}

#pragma region IAudioFrameInfoCollection
AVSampleFormat video::AVStreamWrapper::SampleFormat() const
{
	return AVSampleFormat(_wrapped_obj->codecpar->format);
}

void video::AVStreamWrapper::SetSampleFormat(AVSampleFormat value)
{
	_wrapped_obj->codecpar->format = value;
}

AVChannelLayout video::AVStreamWrapper::ChannelLayout() const
{
	return _wrapped_obj->codecpar->ch_layout;
}

void video::AVStreamWrapper::SetChannelLayout(AVChannelLayout value)
{
	_wrapped_obj->codecpar->ch_layout = value;
}

int video::AVStreamWrapper::SampleRate() const
{
	return _wrapped_obj->codecpar->sample_rate;
}

void video::AVStreamWrapper::SetSampleRate(int value)
{
	_wrapped_obj->codecpar->sample_rate = value;
}
#pragma endregion

#pragma region IVideoStreamInfoCollection
int video::AVStreamWrapper::Width() const
{
	return _wrapped_obj->codecpar->width;
}
void video::AVStreamWrapper::SetWidth(int value)
{
	_wrapped_obj->codecpar->width = value;
}

int video::AVStreamWrapper::Height() const
{
	return _wrapped_obj->codecpar->height;
}
void video::AVStreamWrapper::SetHeight(int value)
{
	_wrapped_obj->codecpar->height = value;
}

AVPixelFormat video::AVStreamWrapper::PixelFormat() const
{
	return (AVPixelFormat)_wrapped_obj->codecpar->format;
}
void video::AVStreamWrapper::SetPixelFormat(AVPixelFormat value)
{
	_wrapped_obj->codecpar->format = value;
}

AVRational video::AVStreamWrapper::TimeBase() const
{
	return _wrapped_obj->time_base;
}
void video::AVStreamWrapper::SetTimeBase(AVRational value)
{
	_wrapped_obj->time_base = value;
}

AVRational video::AVStreamWrapper::FrameRate() const
{
	return _wrapped_obj->avg_frame_rate;
}

void video::AVStreamWrapper::SetFrameRate(AVRational value)
{
	_wrapped_obj->avg_frame_rate = value;
}
#pragma endregion

