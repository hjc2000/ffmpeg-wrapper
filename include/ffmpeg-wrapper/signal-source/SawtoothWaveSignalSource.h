#pragma once
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/signal-source/PeriodicSignalSource.h>

namespace video
{
	/**
	 * @brief 锯齿波信号源
	*/
	class SawtoothWaveSignalSource :public PeriodicSignalSource
	{
	public:
		SawtoothWaveSignalSource(double signal_freq, int sample_rate, bool truncate_time = true)
			:PeriodicSignalSource(signal_freq, sample_rate, truncate_time)
		{
		}

		/**
		 * @brief 斜率
		 * @return
		*/
		double slope()
		{
			/* 正向的锯齿波，从 0 增长到 1.0 后瞬间跌落回 0，斜率就是 1.0 / T
			*/
			return 1.0 / signal_period();
		}

	protected:
		// 通过 PeriodicSignalSource 继承
		double sample(double t) override;
	};
}