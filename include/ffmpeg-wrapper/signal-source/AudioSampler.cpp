#include "AudioSampler.h"

video::AudioSampler::AudioSampler(
	std::shared_ptr<base::ISignalSource<double, double>> signal_source,
	video::AudioFrameInfoCollection const &infos)
{
	_signal_source = signal_source;
	IAudioFrameInfoCollection::operator=(infos);
}

int video::AudioSampler::ReadFrame(AVFrameWrapper &frame)
{
	if (frame.IsPlanar())
	{
		throw std::invalid_argument("传进来的帧的声道布局必须是交织类型的");
	}

	return 0;
}

#pragma region IAudioFrameInfoCollection
AVRational video::AudioSampler::TimeBase() const
{
	return _audio_frame_infos.TimeBase();
}

void video::AudioSampler::SetTimeBase(AVRational value)
{
	_audio_frame_infos.SetTimeBase(value);
}

AVSampleFormat video::AudioSampler::SampleFormat() const
{
	return AVSampleFormat::AV_SAMPLE_FMT_DBL;
}

void video::AudioSampler::SetSampleFormat(AVSampleFormat value)
{
	// 采样格式只能是 AVSampleFormat::AV_SAMPLE_FMT_DBL，所以设置不起作用。
}

int video::AudioSampler::SampleRate() const
{
	return _audio_frame_infos.SampleRate();
}

void video::AudioSampler::SetSampleRate(int value)
{
	_audio_frame_infos.SetSampleRate(value);
}

AVChannelLayout video::AudioSampler::ChannelLayout() const
{
	return _audio_frame_infos.ChannelLayout();
}

void video::AudioSampler::SetChannelLayout(AVChannelLayout value)
{
	_audio_frame_infos.SetChannelLayout(value);
}

int video::AudioSampler::SampleCount() const
{
	return _audio_frame_infos.SampleCount();
}

void video::AudioSampler::SetSampleCount(int value)
{
	_audio_frame_infos.SetSampleCount(value);
}
#pragma endregion
