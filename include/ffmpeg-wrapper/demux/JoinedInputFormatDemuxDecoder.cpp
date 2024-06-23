#include "JoinedInputFormatDemuxDecoder.h"
#include <base/task/CancellationToken.h>
#include <ffmpeg-wrapper/factory/DecoderPipeFactory.h>
#include <ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include <ffmpeg-wrapper/input-format/InputFormat.h>
#include <ffmpeg-wrapper/pipe/InfinitePacketPipe.h>
#include <ffmpeg-wrapper/pipe/PacketPump.h>
#include <ffmpeg-wrapper/pipe/ThreadDecoderPipe.h>

using namespace video;
using namespace std;

class video::JoinedInputFormatDemuxDecoder::Context
{
public:
	std::shared_ptr<InputFormat> _current_input_format;
	AVStreamInfoCollection _video_stream_infos;
	std::shared_ptr<IDecoderPipe> _video_decode_pipe;
	int _source_video_stream_index = -1;
	AVStreamInfoCollection _audio_stream_infos;
	std::shared_ptr<IDecoderPipe> _audio_decode_pipe;
	int _source_audio_stream_index = -1;
	std::shared_ptr<InfinitePacketPipe> _infinite_packet_pipe{new InfinitePacketPipe{}};
	base::List<std::shared_ptr<IFrameConsumer>> _video_frame_consumer_list;
	base::List<std::shared_ptr<IFrameConsumer>> _audio_frame_consumer_list;
};

JoinedInputFormatDemuxDecoder::JoinedInputFormatDemuxDecoder()
	: _context(new Context{})
{
}

void video::JoinedInputFormatDemuxDecoder::InitializeVideoDecoderPipe()
{
	// 如果有视频流，初始化视频解码管道
	AVStreamWrapper stream = _context->_current_input_format->FindBestStream(
		AVMediaType::AVMEDIA_TYPE_VIDEO);

	if (!stream.IsNull())
	{
		_context->_video_stream_infos = stream;
		_context->_source_video_stream_index = stream.Index();

		/* 视频流索引设置为 0. 向 _video_decode_pipe
		 * 送入包时也必须先将包的索引设置为 0. DecoderPipe 的 SendPacket
		 * 方法会对送入的包进行过滤。如果索引号不匹配，不会解码。
		 */
		_context->_video_stream_infos.SetIndex(0);
		// 经过 InfinitePacketPipe 处理后时间基会变成 1/90000，所以这里需要改。
		_context->_video_stream_infos.SetTimeBase(AVRational{1, 90000});

		_context->_video_decode_pipe = shared_ptr<ThreadDecoderPipe>{
			new ThreadDecoderPipe{
				video::DecoderPipeFactory::Instance(),
				_context->_video_stream_infos}};

		_context->_video_decode_pipe->FrameConsumerList().Add(
			_context->_video_frame_consumer_list);
		_context->_infinite_packet_pipe->PacketConsumerList().Add(
			_context->_video_decode_pipe);
	}
	else
	{
		_context->_video_decode_pipe = nullptr;
		_context->_source_video_stream_index = -1;
	}
}

void video::JoinedInputFormatDemuxDecoder::InitializeAudioDecoderPipe()
{
	// 如果有音频流，初始化音频解码管道
	AVStreamWrapper stream = _context->_current_input_format->FindBestStream(
		AVMediaType::AVMEDIA_TYPE_AUDIO);

	if (!stream.IsNull())
	{
		_context->_audio_stream_infos = stream;
		_context->_source_audio_stream_index = stream.Index();

		/* 音频流的索引设置为 1. 向 _audio_decode_pipe
		 * 送入包时也必须先将包的索引设置为 1. DecoderPipe 的 SendPacket
		 * 方法会对送入的包进行过滤。如果索引号不匹配，不会解码。
		 */
		_context->_audio_stream_infos.SetIndex(1);
		// 经过 InfinitePacketPipe 处理后时间基会变成 1/90000，所以这里需要改。
		_context->_audio_stream_infos.SetTimeBase(AVRational{1, 90000});

		_context->_audio_decode_pipe = shared_ptr<ThreadDecoderPipe>{
			new ThreadDecoderPipe{
				video::DecoderPipeFactory::Instance(),
				_context->_audio_stream_infos,
			},
		};

		_context->_audio_decode_pipe->FrameConsumerList().Add(
			_context->_audio_frame_consumer_list);

		_context->_infinite_packet_pipe->PacketConsumerList().Add(
			_context->_audio_decode_pipe);
	}
	else
	{
		_context->_audio_decode_pipe = nullptr;
		_context->_source_audio_stream_index = -1;
	}
}

void video::JoinedInputFormatDemuxDecoder::OpenInputIfNull()
{
	if (_context->_current_input_format == nullptr &&
		_get_format_callback != nullptr)
	{
		_context->_current_input_format = _get_format_callback();
	}

	if (_context->_current_input_format == nullptr)
	{
		return;
	}

	// 成功打开新的输入格式。
	if (_context->_video_decode_pipe)
	{
		_context->_video_decode_pipe->FlushDecoderButNotFlushConsumers();
	}

	if (_context->_audio_decode_pipe)
	{
		_context->_audio_decode_pipe->FlushDecoderButNotFlushConsumers();
	}

	_context->_infinite_packet_pipe->PacketConsumerList().Clear();
	InitializeVideoDecoderPipe();
	InitializeAudioDecoderPipe();
}

void video::JoinedInputFormatDemuxDecoder::Pump(
	shared_ptr<base::CancellationToken> cancel_pump)
{
	while (!cancel_pump->IsCancellationRequested())
	{
		OpenInputIfNull();
		if (_context->_current_input_format == nullptr)
		{
			_context->_infinite_packet_pipe->FlushConsumer();
			return;
		}

		shared_ptr<PacketPump> packet_pump{new PacketPump{_context->_current_input_format}};
		packet_pump->PacketConsumerList().Add(_context->_infinite_packet_pipe);
		packet_pump->_on_before_send_packet_to_consumer = [&](AVPacketWrapper *packet)
		{
			if (!packet)
			{
				return;
			}

			packet->ChangeTimeBase(AVRational{1, 90000});
			if (packet->StreamIndex() == _context->_source_video_stream_index)
			{
				// 视频流的索引更改为 0.
				packet->SetStreamIndex(0);
			}
			else if (packet->StreamIndex() == _context->_source_audio_stream_index)
			{
				// 音频流的索引更改为 1.
				packet->SetStreamIndex(1);
			}
		};

		packet_pump->Pump(cancel_pump);
		_context->_current_input_format = nullptr;
	}
}

void video::JoinedInputFormatDemuxDecoder::AddVideoFrameConsumer(
	shared_ptr<IFrameConsumer> consumer)
{
	_context->_video_frame_consumer_list.Add(consumer);
}

void video::JoinedInputFormatDemuxDecoder::AddAudioFrameConsumer(
	shared_ptr<IFrameConsumer> consumer)
{
	_context->_audio_frame_consumer_list.Add(consumer);
}
