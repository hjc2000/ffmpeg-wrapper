#include "IVideoFrameInfoCollection.h"

using namespace video;

IVideoFrameInfoCollection &video::IVideoFrameInfoCollection::operator=(IVideoFrameInfoCollection const &o)
{
	SetWidth(o.Width());
	SetHeight(o.Height());
	SetPixelFormat(o.PixelFormat());
	return *this;
}

bool IVideoFrameInfoCollection::operator==(IVideoFrameInfoCollection const &o) const
{
	return Width() == o.Width() &&
		   Height() == o.Height() &&
		   PixelFormat() == o.PixelFormat();
}
