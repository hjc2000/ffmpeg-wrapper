#include "BestStreamDemuxDecoder.h"
#include <base/pipe/Pump.h>

using namespace video;

video::BestStreamDemuxDecoder::BestStreamDemuxDecoder(std::shared_ptr<InputFormat> input_format)
{
    _input_format = input_format;
    AVStreamWrapper stream = _input_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
    if (!stream.IsNull())
    {
        _video_decode_pipe = std::shared_ptr<ThreadDecoderPipe>{new ThreadDecoderPipe{stream}};
    }

    stream = _input_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
    if (!stream.IsNull())
    {
        _audio_decode_pipe = std::shared_ptr<ThreadDecoderPipe>{new ThreadDecoderPipe{stream}};
    }
}

void video::BestStreamDemuxDecoder::AddVideoFrameConsumer(std::shared_ptr<base::IConsumer<AVFrameWrapper>> consumer)
{
    if (_video_decode_pipe)
    {
        _video_decode_pipe->ConsumerList().Add(consumer);
    }
}

void video::BestStreamDemuxDecoder::AddAudioFrameConsumer(std::shared_ptr<base::IConsumer<AVFrameWrapper>> consumer)
{
    if (_audio_decode_pipe)
    {
        _audio_decode_pipe->ConsumerList().Add(consumer);
    }
}

void video::BestStreamDemuxDecoder::PumpDataToConsumers(std::shared_ptr<base::CancellationToken> cancel_pump)
{
    std::shared_ptr<base::Pump<AVPacketWrapper>> packet_pump{new base::Pump<AVPacketWrapper>{_input_format}};
    if (_video_decode_pipe)
    {
        packet_pump->ConsumerList().Add(_video_decode_pipe);
    }

    if (_audio_decode_pipe)
    {
        packet_pump->ConsumerList().Add(_audio_decode_pipe);
    }

    packet_pump->PumpDataToConsumers(cancel_pump);
}
