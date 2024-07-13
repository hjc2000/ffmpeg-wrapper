#include "IAudioFrameInfoCollection.h"

using namespace video;

IAudioFrameInfoCollection &video::IAudioFrameInfoCollection::operator=(IAudioFrameInfoCollection const &o)
{
	IAudioStreamInfoCollection::operator=(o);
	SetSampleCount(o.SampleCount());
	return *this;
}

bool IAudioFrameInfoCollection::operator==(IAudioFrameInfoCollection const &o) const
{
	return IAudioStreamInfoCollection::operator==(o) &&
		   SampleCount() == o.SampleCount();
}
