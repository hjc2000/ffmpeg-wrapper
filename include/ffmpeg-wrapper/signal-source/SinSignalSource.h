#pragma once
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/signal-source/PeriodicSignalSource.h>

namespace video
{
	/**
	 * @brief 用于生成对正弦波，设置好参数后就能开始模拟对正弦波的采样了。调用一个方法即可获取采样值
	*/
	class SinSignalSource :public PeriodicSignalSource
	{
	public:
		SinSignalSource(double signal_freq, int sample_rate, bool truncate_time = true)
			:PeriodicSignalSource(signal_freq, sample_rate, truncate_time)
		{
		}

		double sample(double t) override
		{
			// y = sin(2 * pi * f * t)
			return sin(2 * M_PI * _signal_freq * t);
		}
	};
}