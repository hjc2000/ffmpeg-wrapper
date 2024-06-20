#include"IVideoFrameInfoCollection.h"

using namespace video;

IVideoFrameInfoCollection &video::IVideoFrameInfoCollection::operator=(IVideoFrameInfoCollection const &another)
{
	SetWidth(another.Width());
	SetHeight(another.Height());
	SetPixelFormat(another.PixelFormat());
	return *this;
}

bool IVideoFrameInfoCollection::operator==(IVideoFrameInfoCollection const &another) const
{
	return Width() == another.Width() &&
		Height() == another.Height() &&
		PixelFormat() == another.PixelFormat();
}
