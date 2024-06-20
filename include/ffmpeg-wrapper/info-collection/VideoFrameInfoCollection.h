#pragma once
#include<ffmpeg-wrapper/info-collection/IVideoFrameInfoCollection.h>

namespace video
{
	class VideoFrameInfoCollection :public IVideoFrameInfoCollection
	{
	public:
		VideoFrameInfoCollection() = default;

		VideoFrameInfoCollection(IVideoFrameInfoCollection const &another)
		{
			IVideoFrameInfoCollection::operator=(another);
		}

		VideoFrameInfoCollection &operator=(IVideoFrameInfoCollection const &value)
		{
			IVideoFrameInfoCollection::operator=(value);
			return *this;
		}

		int Width() const override;
		void SetWidth(int value) override;

		int Height() const override;
		void SetHeight(int value) override;

		AVPixelFormat PixelFormat() const override;
		void SetPixelFormat(AVPixelFormat value) override;

		int _width = 0;
		int _height = 0;
		AVPixelFormat _pixel_format{};
	};
}