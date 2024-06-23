#include "Sampler.h"

video::Sampler::Sampler(
	std::shared_ptr<base::ISignalSource<double, double>> signal_source,
	video::AudioFrameInfoCollection const &infos)
{
	_signal_source = signal_source;
}

int video::Sampler::ReadFrame(AVFrameWrapper &frame)
{
	return 0;
}

#pragma region IAudioFrameInfoCollection
AVRational video::Sampler::TimeBase() const
{
	return AVRational();
}

void video::Sampler::SetTimeBase(AVRational value)
{
}

AVSampleFormat video::Sampler::SampleFormat() const
{
	return AVSampleFormat();
}

void video::Sampler::SetSampleFormat(AVSampleFormat value)
{
}

int video::Sampler::SampleRate() const
{
	return 0;
}

void video::Sampler::SetSampleRate(int value)
{
}

AVChannelLayout video::Sampler::ChannelLayout() const
{
	return AVChannelLayout();
}

void video::Sampler::SetChannelLayout(AVChannelLayout value)
{
}

int video::Sampler::SampleCount() const
{
	return 0;
}

void video::Sampler::SetSampleCount(int value)
{
}
#pragma endregion
