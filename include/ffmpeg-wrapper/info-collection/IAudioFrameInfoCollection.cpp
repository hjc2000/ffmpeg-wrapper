#include"IAudioFrameInfoCollection.h"

using namespace video;

IAudioFrameInfoCollection &video::IAudioFrameInfoCollection::operator=(IAudioFrameInfoCollection const &value)
{
	IAudioStreamInfoCollection::operator=(value);
	SetSampleCount(value.SampleCount());
	return *this;
}

bool IAudioFrameInfoCollection::operator==(IAudioFrameInfoCollection const &another) const
{
	return IAudioStreamInfoCollection::operator==(another) &&
		SampleCount() == another.SampleCount();
}
