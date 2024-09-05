#pragma once
#include <ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include <memory>

namespace video
{
    /// @brief 音频流信息集合。
    class AudioStreamInfoCollection :
        public IAudioStreamInfoCollection
    {
    public:
        AudioStreamInfoCollection() = default;
        AudioStreamInfoCollection(AudioStreamInfoCollection const &o);
        AudioStreamInfoCollection(std::shared_ptr<IAudioStreamInfoCollection> o);
        AudioStreamInfoCollection(IAudioStreamInfoCollection const &o);

        AudioStreamInfoCollection &operator=(AudioStreamInfoCollection const &o);

        AVRational _time_base{};
        AVSampleFormat _sample_format{};
        int _sample_rate = 0;
        AVChannelLayout _ch_layout{};

#pragma region IAudioStreamInfoCollection

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

#pragma endregion
    };

} // namespace video
