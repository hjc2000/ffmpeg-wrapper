#include "IVideoStreamInfoCollection.h"
#include <ffmpeg-wrapper/AVCalculate.h>

using namespace video;

IVideoStreamInfoCollection &video::IVideoStreamInfoCollection::operator=(IVideoStreamInfoCollection const &o)
{
	IVideoFrameInfoCollection::operator=(o);
	SetTimeBase(o.TimeBase());
	SetFrameRate(o.FrameRate());
	return *this;
}

base::Fraction video::IVideoStreamInfoCollection::FrameInterval() const
{
	return video::AVRationalToFraction(FrameRate()).Reciprocal();
}

bool IVideoStreamInfoCollection::operator==(IVideoStreamInfoCollection const &o) const
{
	return IVideoFrameInfoCollection::operator==(o) &&
		   TimeBase() == o.TimeBase() &&
		   FrameRate() == o.FrameRate();
}
