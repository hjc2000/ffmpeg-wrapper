#include<ffmpeg-wrapper/info-collection/VideoFrameInfoCollection.h>

int video::VideoFrameInfoCollection::Width() const
{
	return _width;
}

void video::VideoFrameInfoCollection::SetWidth(int value)
{
	_width = value;
}

int video::VideoFrameInfoCollection::Height() const
{
	return _height;
}

void video::VideoFrameInfoCollection::SetHeight(int value)
{
	_height = value;
}

AVPixelFormat video::VideoFrameInfoCollection::PixelFormat() const
{
	return _pixel_format;
}

void video::VideoFrameInfoCollection::SetPixelFormat(AVPixelFormat value)
{
	_pixel_format = value;
}
