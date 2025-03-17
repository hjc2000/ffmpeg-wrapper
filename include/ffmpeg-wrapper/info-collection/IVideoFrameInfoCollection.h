#pragma once
#include <ffmpeg-wrapper/ffmpeg_headers.h>

namespace video
{
	/// @brief 视频帧信息集合接口。
	class IVideoFrameInfoCollection
	{
	public:
		virtual ~IVideoFrameInfoCollection() = default;
		IVideoFrameInfoCollection &operator=(IVideoFrameInfoCollection const &o);

		virtual int Width() const = 0;
		virtual void SetWidth(int value) = 0;

		virtual int Height() const = 0;
		virtual void SetHeight(int value) = 0;

		virtual AVPixelFormat PixelFormat() const = 0;
		virtual void SetPixelFormat(AVPixelFormat value) = 0;

		bool operator==(IVideoFrameInfoCollection const &o) const;
	};
} // namespace video
