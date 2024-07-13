#pragma once
#include <ffmpeg-wrapper/AVCompare.h>
#include <ffmpeg-wrapper/base_include.h>
#include <ffmpeg-wrapper/info-collection/IVideoFrameInfoCollection.h>

namespace video
{
	/// @brief 视频流信息集合接口。
	/// @note 视频流比起视频帧多了 2 个信息：帧率，时间基。
	class IVideoStreamInfoCollection
		: public IVideoFrameInfoCollection
	{
	public:
		virtual ~IVideoStreamInfoCollection() = default;
		IVideoStreamInfoCollection &operator=(IVideoStreamInfoCollection const &value);

		virtual AVRational TimeBase() const = 0;
		virtual void SetTimeBase(AVRational value) = 0;

		virtual AVRational FrameRate() const = 0;
		virtual void SetFrameRate(AVRational value) = 0;

		uint32_t FrameIntervalInMilliseconds() const;

		using IVideoFrameInfoCollection::operator==;
		bool operator==(IVideoStreamInfoCollection const &another) const;
	};
}
