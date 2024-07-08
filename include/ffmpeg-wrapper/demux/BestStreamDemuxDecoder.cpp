#include "BestStreamDemuxDecoder.h"
#include <ffmpeg-wrapper/pipe/PacketPump.h>

using namespace std;
using namespace video;

video::BestStreamDemuxDecoder::BestStreamDemuxDecoder(shared_ptr<InputFormat> input_format)
{
	_input_format = input_format;
	AVStreamWrapper stream = _input_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
	if (!stream.IsNull())
	{
		_video_decode_pipe = shared_ptr<ThreadDecoderPipe>{new ThreadDecoderPipe{stream}};
	}

	stream = _input_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
	if (!stream.IsNull())
	{
		_audio_decode_pipe = shared_ptr<ThreadDecoderPipe>{new ThreadDecoderPipe{stream}};
	}
}

void video::BestStreamDemuxDecoder::AddVideoFrameConsumer(shared_ptr<base::IConsumer<AVFrameWrapper>> consumer)
{
	if (_video_decode_pipe)
	{
		_video_decode_pipe->ConsumerList().Add(consumer);
	}
}

void video::BestStreamDemuxDecoder::AddAudioFrameConsumer(shared_ptr<base::IConsumer<AVFrameWrapper>> consumer)
{
	if (_audio_decode_pipe)
	{
		_audio_decode_pipe->ConsumerList().Add(consumer);
	}
}

void video::BestStreamDemuxDecoder::Pump(shared_ptr<base::CancellationToken> cancel_pump)
{
	shared_ptr<PacketPump> packet_pump{new PacketPump{_input_format}};
	if (_video_decode_pipe)
	{
		packet_pump->ConsumerList().Add(_video_decode_pipe);
	}

	if (_audio_decode_pipe)
	{
		packet_pump->ConsumerList().Add(_audio_decode_pipe);
	}

	packet_pump->Pump(cancel_pump);
}
