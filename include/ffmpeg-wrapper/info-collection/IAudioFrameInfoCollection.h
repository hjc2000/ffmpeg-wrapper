#pragma once
#include "ffmpeg-wrapper/ffmpeg.h"
#include <ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>

namespace video
{
	/// @brief 音频帧信息集合接口。
	/// @note 音频帧和音频流相比，多出了 "采样点个数" 这一条信息。
	class IAudioFrameInfoCollection : public IAudioStreamInfoCollection
	{
	public:
		virtual ~IAudioFrameInfoCollection() = default;
		IAudioFrameInfoCollection &operator=(IAudioFrameInfoCollection const &value);

		virtual int SampleCount() const = 0;
		virtual void SetSampleCount(int value) = 0;

		using IAudioStreamInfoCollection::operator==;
		bool operator==(IAudioFrameInfoCollection const &o) const;
	};
} // namespace video
