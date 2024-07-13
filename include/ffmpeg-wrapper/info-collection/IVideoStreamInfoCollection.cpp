#include "IVideoStreamInfoCollection.h"

using namespace video;

IVideoStreamInfoCollection &video::IVideoStreamInfoCollection::operator=(IVideoStreamInfoCollection const &o)
{
	IVideoFrameInfoCollection::operator=(o);
	SetTimeBase(o.TimeBase());
	SetFrameRate(o.FrameRate());
	return *this;
}

uint32_t video::IVideoStreamInfoCollection::FrameIntervalInMilliseconds() const
{
	return 1000 * FrameRate().den / FrameRate().num;
}

bool IVideoStreamInfoCollection::operator==(IVideoStreamInfoCollection const &o) const
{
	return IVideoFrameInfoCollection::operator==(o) &&
		   TimeBase() == o.TimeBase() &&
		   FrameRate() == o.FrameRate();
}
