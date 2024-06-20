#pragma once
#include<ffmpeg-wrapper/info-collection/IAudioFrameInfoCollection.h>

namespace video
{
	/// <summary>
	///		简单的储存音频信息的结构体
	/// </summary>
	class AudioFrameInfoCollection :public IAudioFrameInfoCollection
	{
	public:
		AudioFrameInfoCollection() = default;

		AudioFrameInfoCollection(IAudioStreamInfoCollection const &infos, int nb_samples)
		{
			IAudioStreamInfoCollection::operator=(infos);
			_nb_samples = nb_samples;
		}

		AudioFrameInfoCollection(IAudioFrameInfoCollection const &infos)
		{
			IAudioFrameInfoCollection::operator=(infos);
		}

		AudioFrameInfoCollection &operator=(IAudioFrameInfoCollection const &value)
		{
			IAudioFrameInfoCollection::operator=(value);
			return *this;
		}

		AVRational _time_base{};
		AVSampleFormat _sample_format{};
		int _sample_rate = 0;
		AVChannelLayout _ch_layout{};
		int _nb_samples = 0;

		#pragma region IAudioFrameInfoCollection
		AVRational TimeBase() const override
		{
			return _time_base;
		}
		void SetTimeBase(AVRational value) override
		{
			_time_base = value;
		}

		AVSampleFormat SampleFormat() const override
		{
			return _sample_format;
		}
		void SetSampleFormat(AVSampleFormat value) override
		{
			_sample_format = value;
		}

		int SampleRate() const override
		{
			return _sample_rate;
		}
		void SetSampleRate(int value) override
		{
			_sample_rate = value;
		}

		AVChannelLayout ChannelLayout() const override
		{
			return _ch_layout;
		}
		void SetChannelLayout(AVChannelLayout value) override
		{
			_ch_layout = value;
		}

		int SampleCount() const override
		{
			return _nb_samples;
		}
		void SetSampleCount(int value) override
		{
			_nb_samples = value;
		}
		#pragma endregion

	};
}