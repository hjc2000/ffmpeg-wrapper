#pragma once
#include<ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>

namespace video
{
	class AudioStreamInfoCollection :
		public IAudioStreamInfoCollection
	{
	public:
		AudioStreamInfoCollection() = default;

		AudioStreamInfoCollection(shared_ptr<IAudioStreamInfoCollection> value)
		{
			*this = *value;
		}

		AudioStreamInfoCollection(IAudioStreamInfoCollection const &value)
		{
			*this = value;
		}

		AudioStreamInfoCollection &operator=(IAudioStreamInfoCollection const &value)
		{
			IAudioStreamInfoCollection::operator=(value);
			return *this;
		}

		AVRational _time_base{};
		AVSampleFormat _sample_format{};
		int _sample_rate = 0;
		AVChannelLayout _ch_layout{};

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
	};

}