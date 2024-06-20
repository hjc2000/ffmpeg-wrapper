#include"ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h"

using namespace video;

AVStreamInfoCollection::AVStreamInfoCollection(AVStreamInfoCollection const &stream)
{
	*this = stream;
}

AVStreamInfoCollection::AVStreamInfoCollection(AVStreamWrapper const &stream)
{
	*this = stream;
}

AVStreamInfoCollection::~AVStreamInfoCollection()
{
	avcodec_parameters_free(&_codec_params);
}

AVStreamInfoCollection &AVStreamInfoCollection::operator=(AVStreamInfoCollection const &stream)
{
	_index = stream._index;
	_media_type = stream._media_type;
	_bitrate = stream._bitrate;
	CopyCodecParamFrom(stream._codec_params);
	_codec = stream._codec;

	_timebase = stream._timebase;
	_sample_format = stream._sample_format;
	_sample_rate = stream._sample_rate;
	_ch_layout = stream._ch_layout;
	_width = stream._width;
	_height = stream._height;
	_pixel_format = stream._pixel_format;
	_frame_rate = stream._frame_rate;

	return *this;
}

AVStreamInfoCollection &AVStreamInfoCollection::operator=(AVStreamWrapper const &stream)
{
	_index = stream.Index();
	_media_type = stream.MediaType();
	_bitrate = stream.Bitrate();
	CopyCodecParamFrom(stream->codecpar);
	_codec = stream.Codec();

	_timebase = stream.TimeBase();
	_sample_format = stream.SampleFormat();
	_sample_rate = stream.SampleRate();
	_ch_layout = stream.ChannelLayout();
	_width = stream.Width();
	_height = stream.Height();
	_pixel_format = stream.PixelFormat();
	_frame_rate = stream.FrameRate();

	return *this;
}

void video::AVStreamInfoCollection::CopyCodecParamFrom(AVCodecParameters const *src)
{
	// avcodec_parameters_copy 会先释放 dst，然后再将 src 的数据复制到 dst。
	avcodec_parameters_copy(_codec_params, src);
}

int video::AVStreamInfoCollection::Index() const
{
	return _index;
}

void video::AVStreamInfoCollection::SetIndex(int value)
{
	_index = value;
}

AVRational video::AVStreamInfoCollection::TimeBase() const
{
	return _timebase;
}

void video::AVStreamInfoCollection::SetTimeBase(AVRational value)
{
	_timebase = value;
}

AVSampleFormat video::AVStreamInfoCollection::SampleFormat() const
{
	return _sample_format;
}

void video::AVStreamInfoCollection::SetSampleFormat(AVSampleFormat value)
{
	_sample_format = value;
}

int video::AVStreamInfoCollection::SampleRate() const
{
	return _sample_rate;
}

void video::AVStreamInfoCollection::SetSampleRate(int value)
{
	_sample_rate = value;
}

AVChannelLayout video::AVStreamInfoCollection::ChannelLayout() const
{
	return _ch_layout;
}

void video::AVStreamInfoCollection::SetChannelLayout(AVChannelLayout value)
{
	_ch_layout = value;
}

int video::AVStreamInfoCollection::Width() const
{
	return _width;
}

void video::AVStreamInfoCollection::SetWidth(int value)
{
	_width = value;
}

int video::AVStreamInfoCollection::Height() const
{
	return _height;
}

void video::AVStreamInfoCollection::SetHeight(int value)
{
	_height = value;
}

AVPixelFormat video::AVStreamInfoCollection::PixelFormat() const
{
	return _pixel_format;
}

void video::AVStreamInfoCollection::SetPixelFormat(AVPixelFormat value)
{
	_pixel_format = value;
}

AVRational video::AVStreamInfoCollection::FrameRate() const
{
	return _frame_rate;
}

void video::AVStreamInfoCollection::SetFrameRate(AVRational value)
{
	_frame_rate = value;
}
