#include "ffmpeg-wrapper/signal-source/PeriodicSignalSource.h"
#include<ffmpeg-wrapper/AVChannelLayoutExtension.h>

using namespace video;

double video::PeriodicSignalSource::sample()
{
	// 保存当前时间
	double current_t = _sample_t;

	// 递增采样时间，为下一次采样做准备
	_sample_t += sample_interval();

	// 截断时间
	if (_truncate_time)
	{
		// https://www.yuque.com/qiaodangyi/yrin4p/mfwzxoy0dicvbke0
		_sample_t = truncate_time_into_a_period(_sample_t);
	}

	// y = sin(2 * pi * f * t)
	return sample(current_t);
}

void video::PeriodicSignalSource::SetSampleRate(int value)
{
	throw std::runtime_error{ "不支持的操作" };
}

AVRational video::PeriodicSignalSource::TimeBase() const
{
	return AVRational{ 1,SampleRate() };
}

void video::PeriodicSignalSource::SetTimeBase(AVRational value)
{
	throw std::runtime_error{ "不支持的操作" };
}

AVSampleFormat video::PeriodicSignalSource::SampleFormat()
{
	return AVSampleFormat::AV_SAMPLE_FMT_DBL;
}

void video::PeriodicSignalSource::SetSampleFormat(AVSampleFormat value)
{
	throw std::runtime_error{ "不支持的操作" };
}

AVChannelLayout video::PeriodicSignalSource::ChannelLayout() const
{
	return _ch_layout;
}

void video::PeriodicSignalSource::SetChannelLayout(AVChannelLayout value)
{
	_ch_layout = value;
}

int video::PeriodicSignalSource::SampleCount() const
{
	return _nb_samples;
}

void video::PeriodicSignalSource::SetSampleCount(int value)
{
	_nb_samples = value;
}
