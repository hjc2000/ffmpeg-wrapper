#include "IAudioStreamInfoCollection.h"
#include <ffmpeg-wrapper/AVChannelLayoutExtension.h>

using namespace video;

IAudioStreamInfoCollection &video::IAudioStreamInfoCollection::operator=(IAudioStreamInfoCollection const &value)
{
	SetTimeBase(value.TimeBase());
	SetSampleFormat(value.SampleFormat());
	SetSampleRate(value.SampleRate());
	SetChannelLayout(value.ChannelLayout());
	return *this;
}

std::string video::IAudioStreamInfoCollection::sample_format_string() const
{
	const char *name = av_get_sample_fmt_name(SampleFormat());
	return name ? std::string(name) : "";
}

std::string video::IAudioStreamInfoCollection::channel_layout_description() const
{
	return AVChannelLayoutExtension::channel_layout_description(ChannelLayout());
}

bool video::IAudioStreamInfoCollection::IsPlanar() const
{
	return av_sample_fmt_is_planar(SampleFormat());
}

int video::IAudioStreamInfoCollection::BytesPerSample() const
{
	return av_get_bytes_per_sample(SampleFormat());
}

bool IAudioStreamInfoCollection::operator==(IAudioStreamInfoCollection const &another) const
{
	return TimeBase() == another.TimeBase() &&
		   SampleFormat() == another.SampleFormat() &&
		   SampleRate() == another.SampleRate() &&
		   ChannelLayout() == another.ChannelLayout();
}

double video::IAudioStreamInfoCollection::DoubleSampleInterval() const
{
	return 1.0 / SampleRate();
}

double video::IAudioStreamInfoCollection::sample_interval_in_milliseconds() const
{
	return 1.0 * 1000 / SampleRate();
}
