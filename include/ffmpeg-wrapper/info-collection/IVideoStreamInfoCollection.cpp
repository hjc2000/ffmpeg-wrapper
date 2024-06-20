#include"IVideoStreamInfoCollection.h"

using namespace video;

IVideoStreamInfoCollection &video::IVideoStreamInfoCollection::operator=(IVideoStreamInfoCollection const &value)
{
	IVideoFrameInfoCollection::operator=(value);
	SetTimeBase(value.TimeBase());
	SetFrameRate(value.FrameRate());
	return *this;
}

uint32_t video::IVideoStreamInfoCollection::FrameIntervalInMilliseconds() const
{
	return 1000 * FrameRate().den / FrameRate().num;
}

bool IVideoStreamInfoCollection::operator==(IVideoStreamInfoCollection const &another) const
{
	return IVideoFrameInfoCollection::operator==(another) &&
		TimeBase() == another.TimeBase() &&
		FrameRate() == another.FrameRate();
}
