#include "IAudioStreamInfoCollection.h"
#include <ffmpeg-wrapper/AVChannelLayoutExtension.h>
#include <ffmpeg-wrapper/AVCompare.h>

using namespace video;

IAudioStreamInfoCollection &video::IAudioStreamInfoCollection::operator=(IAudioStreamInfoCollection const &value)
{
	SetTimeBase(value.TimeBase());
	SetSampleFormat(value.SampleFormat());
	SetSampleRate(value.SampleRate());
	SetChannelLayout(value.ChannelLayout());
	return *this;
}

base::Fraction video::IAudioStreamInfoCollection::SampleInterval() const
{
	return base::Fraction{1, SampleRate()};
}

std::string video::IAudioStreamInfoCollection::sample_format_string() const
{
	const char *name = av_get_sample_fmt_name(SampleFormat());
	return name ? std::string(name) : "";
}

std::string video::IAudioStreamInfoCollection::ChannelLayoutDescription() const
{
	return AVChannelLayoutExtension::ChannelLayoutDescription(ChannelLayout());
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
