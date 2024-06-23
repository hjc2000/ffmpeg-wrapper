#pragma once
#include <base/signal/ISignalSource.h>
#include <ffmpeg-wrapper/info-collection/AudioFrameInfoCollection.h>
#include <ffmpeg-wrapper/pipe/interface/IFrameSource.h>
#include <memory>

namespace video
{
	/// @brief 采样器。从信号源中采样，生成音频帧。
	class Sampler
		: video::IFrameSource
	{
	private:
		std::shared_ptr<base::ISignalSource<double, double>> _signal_source;

	public:
		Sampler(std::shared_ptr<base::ISignalSource<double, double>> signal_source);
		int ReadFrame(AVFrameWrapper &frame) override;
	};
}
