#pragma once
#include<ffmpeg-wrapper/AVChannelLayoutExtension.h>
#include<ffmpeg-wrapper/AVCompare.h>
#include<ffmpeg-wrapper/AVSampleFormatExtention.h>
#include<ffmpeg-wrapper/base_include.h>

namespace video
{
	/// <summary>
	///		继承此接口表示含有：采样格式、采样率、声道布局、时间基 等信息。
	/// </summary>
	class IAudioStreamInfoCollection
	{
	public:
		virtual ~IAudioStreamInfoCollection() = default;
		IAudioStreamInfoCollection &operator=(IAudioStreamInfoCollection const &value);

		virtual AVRational TimeBase() const = 0;
		virtual void SetTimeBase(AVRational value) = 0;

		virtual AVSampleFormat SampleFormat() const = 0;
		virtual void SetSampleFormat(AVSampleFormat value) = 0;

		virtual int SampleRate() const = 0;
		virtual void SetSampleRate(int value) = 0;

		virtual AVChannelLayout ChannelLayout() const = 0;
		virtual void SetChannelLayout(AVChannelLayout value) = 0;

		/// <summary>
		///		获取两个采样点之间的时间间隔。其实就是采样率的倒数。
		/// </summary>
		/// <returns></returns>
		double sample_interval() const
		{
			return 1.0 / SampleRate();
		}

		/// <summary>
		///		两个采样点之间的时间间隔，单位毫秒。
		///		
		///		本属性内部是
		///		1.0 * 1000 / SampleRate()
		///		即先 1.0 乘 1000 再除以 SampleRate()，这比先除以 SampleRate()
		///		再乘 1000 的精度要高。
		/// </summary>
		/// <returns></returns>
		double sample_interval_in_milliseconds() const
		{
			return 1.0 * 1000 / SampleRate();
		}

		/// <summary>
		///		将 sample_format 属性转化为字符串。
		/// </summary>
		/// <returns>返回 sample_format 的名称。如果是未知类型，返回空字符串</returns>
		string sample_format_string() const
		{
			const char *name = av_get_sample_fmt_name(SampleFormat());
			return name ? string(name) : "";
		}

		/// <summary>
		///		获取声道布局的描述字符串
		/// </summary>
		/// <returns></returns>
		string channel_layout_description() const
		{
			return AVChannelLayoutExtension::channel_layout_description(ChannelLayout());
		}

		/// <summary>
		///		判断本音频帧是不是平面类型。当然，本帧首先得是音频帧。
		/// </summary>
		/// <returns></returns>
		bool IsPlanar() const
		{
			return av_sample_fmt_is_planar(SampleFormat());
		}

		/// <summary>
		///		获取每个采样点的字节数
		/// </summary>
		/// <returns>每个采样点的字节数</returns>
		int BytesPerSample() const
		{
			return av_get_bytes_per_sample(SampleFormat());
		}

		bool operator==(IAudioStreamInfoCollection const &another) const;
	};
}
