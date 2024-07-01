#pragma once
#include <base/pipe/IConsumer.h>
#include <ffmpeg-wrapper/input-format/InputFormat.h>
#include <ffmpeg-wrapper/pipe/ThreadDecoderPipe.h>
#include <ffmpeg-wrapper/pipe/interface/IPump.h>
#include <memory>

namespace video
{
	class BestStreamDemuxDecoder : public IPump
	{
	private:
		std::shared_ptr<InputFormat> _input_format;
		std::shared_ptr<IDecoderPipe> _video_decode_pipe;
		std::shared_ptr<IDecoderPipe> _audio_decode_pipe;

	public:
		BestStreamDemuxDecoder(std::shared_ptr<InputFormat> input_format);

		void AddVideoFrameConsumer(std::shared_ptr<base::IConsumer<AVFrameWrapper>> consumer);
		void AddAudioFrameConsumer(std::shared_ptr<base::IConsumer<AVFrameWrapper>> consumer);

		void Pump(std::shared_ptr<base::CancellationToken> cancel_pump) override;
	};
}
