#pragma once
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/signal-source/ISignalSource.h>
#include<ffmpeg-wrapper/signal-source/PeriodicSignalSource.h>

namespace video
{
	/**
	 * @brief 随机信号
	*/
	class RandomSignalSource :public ISignalSource
	{
	public:
		RandomSignalSource()
		{

		}

		// 通过 ISignalSource 继承
		double sample() override;
	};
}