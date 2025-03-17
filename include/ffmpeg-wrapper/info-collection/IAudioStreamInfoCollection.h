#pragma once
#include <base/math/Fraction.h>
#include <ffmpeg-wrapper/pch.h>
#include <string>

namespace video
{
	/// @brief 音频流信息集合接口。
	class IAudioStreamInfoCollection
	{
	public:
		virtual ~IAudioStreamInfoCollection() = default;
		IAudioStreamInfoCollection &operator=(IAudioStreamInfoCollection const &o);

		virtual AVRational TimeBase() const = 0;
		virtual void SetTimeBase(AVRational value) = 0;

		virtual AVSampleFormat SampleFormat() const = 0;
		virtual void SetSampleFormat(AVSampleFormat value) = 0;

		virtual int SampleRate() const = 0;
		virtual void SetSampleRate(int value) = 0;

		/// @brief 采样时间间隔。单位：秒。
		/// @return
		base::Fraction SampleInterval() const;

		virtual AVChannelLayout ChannelLayout() const = 0;
		virtual void SetChannelLayout(AVChannelLayout value) = 0;

		/// @brief 将 sample_format 属性转化为字符串。
		/// @return 返回 sample_format 的名称。如果是未知类型，返回空字符串
		std::string sample_format_string() const;

		/// @brief 获取声道布局的描述字符串
		/// @return
		std::string ChannelLayoutDescription() const;

		/// @brief 判断本音频帧是不是平面类型。当然，本帧首先得是音频帧。
		/// @return
		bool IsPlanar() const;

		/// @brief 获取每个采样点的字节数
		/// @return 每个采样点的字节数
		int BytesPerSample() const;

		bool operator==(IAudioStreamInfoCollection const &another) const;
	};
} // namespace video
