#include "AudioSampler.h"

video::AudioSampler::AudioSampler(
	std::shared_ptr<base::ISignalSource<double, double>> signal_source,
	video::AudioFrameInfoCollection const &infos)
{
	_signal_source = signal_source;
}

int video::AudioSampler::ReadFrame(AVFrameWrapper &frame)
{
	return 0;
}

#pragma region IAudioFrameInfoCollection
AVRational video::AudioSampler::TimeBase() const
{
	return AVRational();
}

void video::AudioSampler::SetTimeBase(AVRational value)
{
}

AVSampleFormat video::AudioSampler::SampleFormat() const
{
	return AVSampleFormat();
}

void video::AudioSampler::SetSampleFormat(AVSampleFormat value)
{
}

int video::AudioSampler::SampleRate() const
{
	return 0;
}

void video::AudioSampler::SetSampleRate(int value)
{
}

AVChannelLayout video::AudioSampler::ChannelLayout() const
{
	return AVChannelLayout();
}

void video::AudioSampler::SetChannelLayout(AVChannelLayout value)
{
}

int video::AudioSampler::SampleCount() const
{
	return 0;
}

void video::AudioSampler::SetSampleCount(int value)
{
}
#pragma endregion
