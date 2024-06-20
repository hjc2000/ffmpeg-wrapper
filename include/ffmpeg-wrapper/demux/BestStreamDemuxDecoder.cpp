#include"BestStreamDemuxDecoder.h"
#include<ffmpeg-wrapper/factory/DecoderPipeFactory.h>
#include<ffmpeg-wrapper/pipe/PacketPump.h>

using namespace std;
using namespace video;

video::BestStreamDemuxDecoder::BestStreamDemuxDecoder(shared_ptr<InputFormat> input_format)
{
	_input_format = input_format;
	AVStreamWrapper stream = _input_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
	if (!stream.IsNull())
	{
		_video_decode_pipe = shared_ptr<ThreadDecoderPipe> { new ThreadDecoderPipe { video::DecoderPipeFactory::Instance(), stream } };
	}

	stream = _input_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
	if (!stream.IsNull())
	{
		_audio_decode_pipe = shared_ptr<ThreadDecoderPipe> { new ThreadDecoderPipe { video::DecoderPipeFactory::Instance(), stream } };
	}
}

void video::BestStreamDemuxDecoder::AddVideoFrameConsumer(shared_ptr<IFrameConsumer> consumer)
{
	if (_video_decode_pipe)
	{
		_video_decode_pipe->FrameConsumerList().Add(consumer);
	}
}

void video::BestStreamDemuxDecoder::AddAudioFrameConsumer(shared_ptr<IFrameConsumer> consumer)
{
	if (_audio_decode_pipe)
	{
		_audio_decode_pipe->FrameConsumerList().Add(consumer);
	}
}

void video::BestStreamDemuxDecoder::Pump(shared_ptr<base::CancellationToken> cancel_pump)
{
	shared_ptr<PacketPump> packet_pump { new PacketPump { _input_format } };
	if (_video_decode_pipe)
	{
		packet_pump->PacketConsumerList().Add(_video_decode_pipe);
	}

	if (_audio_decode_pipe)
	{
		packet_pump->PacketConsumerList().Add(_audio_decode_pipe);
	}

	packet_pump->Pump(cancel_pump);
}
