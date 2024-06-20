#pragma once
#include<ffmpeg-wrapper/AVSampleFormatExtention.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/signal-source/ISignalSource.h>
#include<ffmpeg-wrapper/wrapper/AVFrameWrapper.h>

namespace video
{
	/**
	 * @brief 周期信号源
	*/
	class PeriodicSignalSource :public ISignalSource
	{
		AVChannelLayout _ch_layout = AVChannelLayout{};
		int _nb_samples = 0;

	protected:
		/**
		 * @brief 将时间截断，让它小于等于一个最小正周期
		 * @param t
		 * @return
		*/
		double truncate_time_into_a_period(double t)
		{
			// 时间是周期的多少倍，截断为整数
			uint64_t multiple = uint64_t(t / signal_period());

			// 减去整数倍个周期
			t = t - multiple * signal_period();
			if (t < 0)
			{
				throw std::underflow_error{ "下溢" };
			}

			return t;
		}

		/**
		 * @brief 信号频率
		*/
		double _signal_freq;

		/**
		 * @brief 采样率
		*/
		int _sample_rate;

		/**
		 * @brief 标识是否截断时间
		*/
		bool _truncate_time;

		/**
		 * @brief 连续时间域的采样时间
		*/
		double _sample_t = 0.0;

		/**
		 * @brief 不考虑时间截断，仅仅根据传入的时间返回一个采样值
		 * @param t
		 * @return
		*/
		virtual double sample(double t) = 0;

	public:
		/**
		 * @brief 构造函数
		 * @param signal_freq 产生的信号的频率
		 * @param sample_rate 采样率
		 * @param truncate_time 是否截断时间。例如 sin 函数是周期函数，y=sin(x) 的 x 大于一个最小正周期后可以截断到小于
		 * 一个最小正周期，所以可以通过截断时间来防止数据溢出，从而达到永久采样而不溢出的效果。
		*/
		PeriodicSignalSource(double signal_freq, int sample_rate, bool truncate_time = true)
		{
			_signal_freq = signal_freq;
			_sample_rate = sample_rate;
			_truncate_time = truncate_time;
		}

		/**
		 * @brief 信号频率
		 * @return
		*/
		double signal_freq()
		{
			return _signal_freq;
		}

		/**
		 * @brief 信号的最小正周期
		 * @return
		*/
		double signal_period()
		{
			return 1.0 / signal_freq();
		}

		/**
		 * @brief 采样的时间间隔
		 * @return
		*/
		double sample_interval()
		{
			return 1.0 / _sample_rate;
		}

		/**
		 * @brief 连续时间域的采样时间。这里是下一次调用 sample 函数时所使用的时间，也就是下一次采样的时间。
		 * 此值受时间截断的影响，如果开启了时间截断，此值会增大到某个值后突然减小
		 * @return 下一次采样的时间
		*/
		double sample_t()
		{
			return _sample_t;
		}

		/**
		 * @brief 采样一次，返回采样结果
		*/
		double sample() override;

		int SampleRate() const
		{
			return _sample_rate;
		}

		/// <summary>
		///		不支持，调用将引发 NotSupportedException
		/// </summary>
		/// <param name="value"></param>
		void SetSampleRate(int value);

		AVRational TimeBase() const override;

		/// <summary>
		///		不支持，调用将引发 NotSupportedException
		/// </summary>
		/// <param name="value"></param>
		void SetTimeBase(AVRational value) override;

		/// <summary>
		///		永远只返回 AVSampleFormat::AV_SAMPLE_FMT_DBL
		/// </summary>
		/// <returns></returns>
		AVSampleFormat SampleFormat();

		/// <summary>
		///		不支持，调用将引发 NotSupportedException
		/// </summary>
		/// <param name="value"></param>
		void SetSampleFormat(AVSampleFormat value) override;

		/// <summary>
		///		因为信号源可以填充任意声道数的交错类型的声道布局，所以本身没有一个固定的 ch_layout。
		///		但是，本类提供了字段，用户可以自由设置 ch_layout 属性。
		/// </summary>
		/// <returns></returns>
		AVChannelLayout ChannelLayout() const override;
		void SetChannelLayout(AVChannelLayout value) override;

		/// <summary>
		///		因为信号源可以填充任意采样点数的帧，所以本身没有一个固定的 nb_samples。但是，本类提供了字段，
		///		用户可以自由设置 nb_samples 属性。
		/// </summary>
		/// <returns></returns>
		int SampleCount() const override;
		void SetSampleCount(int value) override;
	};
}