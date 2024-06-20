#pragma once
#include<ffmpeg-wrapper/base_include.h>

namespace video
{
	class IVideoFrameInfoCollection
	{
	public:
		virtual ~IVideoFrameInfoCollection() = default;
		IVideoFrameInfoCollection &operator=(IVideoFrameInfoCollection const &another);

		virtual int Width() const = 0;
		virtual void SetWidth(int value) = 0;

		virtual int Height() const = 0;
		virtual void SetHeight(int value) = 0;

		virtual AVPixelFormat PixelFormat() const = 0;
		virtual void SetPixelFormat(AVPixelFormat value) = 0;

		bool operator==(IVideoFrameInfoCollection const &another) const;
	};
}
