#pragma once
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/signal-source/PeriodicSignalSource.h>

namespace video
{
	/**
	 * @brief 方波
	*/
	class SquareWaveSignalSource :public PeriodicSignalSource
	{
	public:
		SquareWaveSignalSource(double signal_freq, int sample_rate, double duty_cycle, bool truncate_time = true)
			:PeriodicSignalSource(signal_freq, sample_rate, truncate_time)
		{
			_duty_cycle = duty_cycle;
		}

	protected:
		double sample(double t) override
		{
			t = truncate_time_into_a_period(t);

			if (t / signal_period() <= _duty_cycle)
			{
				// 如果小于占空比
				return 1;
			}

			return 0;
		}

	private:
		/**
		 * @brief 占空比
		*/
		double _duty_cycle;
	};
}