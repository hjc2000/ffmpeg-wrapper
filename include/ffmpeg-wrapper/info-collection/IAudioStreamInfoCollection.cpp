#include"IAudioStreamInfoCollection.h"

using namespace video;

IAudioStreamInfoCollection &video::IAudioStreamInfoCollection::operator=(IAudioStreamInfoCollection const &value)
{
	SetTimeBase(value.TimeBase());
	SetSampleFormat(value.SampleFormat());
	SetSampleRate(value.SampleRate());
	SetChannelLayout(value.ChannelLayout());
	return *this;
}

bool IAudioStreamInfoCollection::operator==(IAudioStreamInfoCollection const &another) const
{
	return TimeBase() == another.TimeBase() &&
		SampleFormat() == another.SampleFormat() &&
		SampleRate() == another.SampleRate() &&
		ChannelLayout() == another.ChannelLayout();
}
