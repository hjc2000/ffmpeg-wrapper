#pragma once
#include <base/math/Fraction.h>
#include <base/signal/ISignalSource.h>
#include <ffmpeg-wrapper/info-collection/AudioFrameInfoCollection.h>
#include <ffmpeg-wrapper/pipe/interface/IFrameSource.h>
#include <memory>

namespace video
{
	/// @brief 采样器。从信号源中采样，生成音频帧。
	class AudioSampler
		: video::IFrameSource,
		  video::IAudioFrameInfoCollection
	{
	private:
		std::shared_ptr<base::ISignalSource<double, double>> _signal_source;
		AudioFrameInfoCollection _audio_frame_infos;
		int64_t _pts = 0;
		base::Fraction _time_base;
		bool _opened = false;

	public:
		AudioSampler(
			std::shared_ptr<base::ISignalSource<double, double>> signal_source,
			video::AudioFrameInfoCollection const &infos);

		void Open();
		int ReadFrame(AVFrameWrapper &frame) override;

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

		AVChannelLayout ChannelLayout() const override;
		void SetChannelLayout(AVChannelLayout value) override;

		int SampleCount() const override;
		void SetSampleCount(int value) override;
#pragma endregion
	};

	void TestAudioSampler();
}
