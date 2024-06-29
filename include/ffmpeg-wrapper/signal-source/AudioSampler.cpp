#include "AudioSampler.h"

video::AudioSampler::AudioSampler(
	std::shared_ptr<base::ISignalSource<double>> signal_source,
	video::AudioFrameInfoCollection const &infos)
{
	_signal_source = signal_source;
	IAudioFrameInfoCollection::operator=(infos);
	_signal_source->SetSampleInterval(base::Fraction{1, SampleRate()});
	_signal_source->Open();
}

void video::AudioSampler::Open()
{
	if (_opened)
	{
		return;
	}

	_opened = true;
}

int video::AudioSampler::ReadFrame(AVFrameWrapper &frame)
{
	if (!_opened)
	{
		throw std::runtime_error{"先打开采样器"};
	}

	if (frame.IsPlanar())
	{
		throw std::invalid_argument("传进来的帧的声道布局必须是交织类型的");
	}

	frame = AVFrameWrapper{_audio_frame_infos};
	double *channel_buffer = reinterpret_cast<double *>(frame->extended_data[0]);

	// 一个音频帧有 SampleCount 个采样点
	for (int i = 0; i < SampleCount(); i++)
	{
		// 采样一次
		double sample_value = _signal_source->Sample();
		_pts++;

		// 为每个声道填充相同的值
		for (int j = 0; j < ChannelLayout().nb_channels; j++)
		{
			channel_buffer[j] = sample_value;
		}

		/* 使指针移动一个偏移量
		 *
		 * 如果传进来的帧的声道布局是交织类型的，则不同声道的采样值是连续放在 extended_data[0]
		 * 这个一维数组中的。假如 extended_data[0] 中存放的是左右双声道的数据，则它的数据布局是：
		 * 		LRLRLR......
		 * 这样交替的。
		 * 采样一次后，为不同声道填充相同的数据，一共有 nb_channels 个声道。
		 * 所以采样一次，就要让指针偏移 nb_samples
		 */
		channel_buffer += ChannelLayout().nb_channels;
	}

	// 返回 0 表示读取帧成功
	return 0;
}

#pragma region IAudioFrameInfoCollection
AVRational video::AudioSampler::TimeBase() const
{
	return _audio_frame_infos.TimeBase();
}

void video::AudioSampler::SetTimeBase(AVRational value)
{
	if (_opened)
	{
		throw std::runtime_error{"采样器打开后不允许更改音频帧属性"};
	}

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
	if (_opened)
	{
		throw std::runtime_error{"采样器打开后不允许更改音频帧属性"};
	}

	_audio_frame_infos.SetSampleRate(value);
}

AVChannelLayout video::AudioSampler::ChannelLayout() const
{
	return _audio_frame_infos.ChannelLayout();
}

void video::AudioSampler::SetChannelLayout(AVChannelLayout value)
{
	if (_opened)
	{
		throw std::runtime_error{"采样器打开后不允许更改音频帧属性"};
	}

	_audio_frame_infos.SetChannelLayout(value);
}

int video::AudioSampler::SampleCount() const
{
	return _audio_frame_infos.SampleCount();
}

void video::AudioSampler::SetSampleCount(int value)
{
	if (_opened)
	{
		throw std::runtime_error{"采样器打开后不允许更改音频帧属性"};
	}

	_audio_frame_infos.SetSampleCount(value);
}
#pragma endregion

#include <ffmpeg-wrapper/factory/EncoderPipeFactory.h>
#include <ffmpeg-wrapper/output-format/FileOutputFormat.h>

void video::TestAudioSampler()
{
	AudioFrameInfoCollection audio_frame_infos;
	audio_frame_infos.SetChannelLayout(AVChannelLayoutExtension::GetDefaultChannelLayout(2));
	audio_frame_infos.SetSampleCount(AVSampleFormatExtention::ParseRequiredSampleCount("eac3"));
	audio_frame_infos.SetSampleFormat(AVSampleFormat::AV_SAMPLE_FMT_DBL);
	audio_frame_infos.SetSampleRate(44100);
	audio_frame_infos.SetTimeBase(AVRational{1, 90000});

	std::shared_ptr<video::FileOutputFormat> out_format{new video::FileOutputFormat{"sin_audio.ts"}};

	video::EncoderPipeFactory::Instance()
		->CreateEncoderPipe(
			"eac3",
			audio_frame_infos,
			out_format);
}
