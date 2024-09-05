#include "ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h"

using namespace video;

void video::AVStreamInfoCollection::CopyCodecParamFrom(AVCodecParameters const *src)
{
    // avcodec_parameters_copy 会先释放 dst，然后再将 src 的数据复制到 dst。
    avcodec_parameters_copy(_codec_params, src);
}

AVStreamInfoCollection::AVStreamInfoCollection(AVStreamInfoCollection const &o)
{
    *this = o;
}

AVStreamInfoCollection::AVStreamInfoCollection(AVStreamWrapper const &o)
{
    _index = o.Index();
    _media_type = o.MediaType();
    _bitrate = o.Bitrate();
    CopyCodecParamFrom(o->codecpar);
    _codec = o.Codec();

    _timebase = o.TimeBase();
    _sample_format = o.SampleFormat();
    _sample_rate = o.SampleRate();
    _ch_layout = o.ChannelLayout();
    _width = o.Width();
    _height = o.Height();
    _pixel_format = o.PixelFormat();
    _frame_rate = o.FrameRate();
}

AVStreamInfoCollection::~AVStreamInfoCollection()
{
    avcodec_parameters_free(&_codec_params);
}

AVStreamInfoCollection &AVStreamInfoCollection::operator=(AVStreamInfoCollection const &o)
{
    _index = o._index;
    _media_type = o._media_type;
    _bitrate = o._bitrate;
    CopyCodecParamFrom(o._codec_params);
    _codec = o._codec;

    _timebase = o._timebase;
    _sample_format = o._sample_format;
    _sample_rate = o._sample_rate;
    _ch_layout = o._ch_layout;
    _width = o._width;
    _height = o._height;
    _pixel_format = o._pixel_format;
    _frame_rate = o._frame_rate;

    return *this;
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
