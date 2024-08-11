#pragma once
#include <base/math/Fraction.h>
#include <base/pipe/ISource.h>
#include <base/signal/ISignalSource.h>
#include <ffmpeg-wrapper/info-collection/AudioFrameInfoCollection.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <memory>

namespace video
{
    /// @brief 采样器。从信号源中采样，生成音频帧。
    class AudioSampler :
        public base::ISource<AVFrameWrapper>,
        public video::IAudioFrameInfoCollection
    {
    private:
        std::shared_ptr<base::ISignalSource<double>> _signal_source;
        AudioFrameInfoCollection _audio_frame_infos;
        base::Fraction _pts = 0;
        bool _opened = false;

    public:
        /// @brief
        /// @param signal_source 传入一个未打开的信号源。
        /// @param infos
        AudioSampler(std::shared_ptr<base::ISignalSource<double>> signal_source,
                     video::IAudioFrameInfoCollection const &infos);

        void Open();
        int ReadData(AVFrameWrapper &frame) override;

#pragma region IAudioFrameInfoCollection
        AVRational TimeBase() const override;
        void SetTimeBase(AVRational value) override;

        /// @brief 采样格式无法自定义，因为采样值是 double，并且只支持交错格式存放，
        /// 所以只能是 AVSampleFormat::AV_SAMPLE_FMT_DBL
        ///
        /// @return
        AVSampleFormat SampleFormat() const override;

        /// @brief 不支持设置采样格式，所以本函数是空函数。
        /// @param value
        void SetSampleFormat(AVSampleFormat value) override;

        int SampleRate() const override;
        void SetSampleRate(int value) override;

        /// @brief 声道布局可以设置，但是所有声道都是相同的采样值。
        /// @return
        AVChannelLayout ChannelLayout() const override;
        void SetChannelLayout(AVChannelLayout value) override;

        int SampleCount() const override;
        void SetSampleCount(int value) override;
#pragma endregion
    };

    /// @brief 测试函数
    void TestAudioSampler();
} // namespace video
