#include"ThreadDecoderPipe.h"
#include<DecoderPipe.h>
#include<iostream>

using namespace std;
using namespace video;

video::ThreadDecoderPipe::ThreadDecoderPipe(std::shared_ptr<IDecoderPipeFactory> factory, AVStreamInfoCollection stream)
{
	_factory = factory;
	_decoder_pipe = _factory->CreateDecoderPipe(stream);
	InitDecodeThread();
}

video::ThreadDecoderPipe::~ThreadDecoderPipe()
{
	if (_disposed) return;
	_disposed = true;

	Dispose();
}

void video::ThreadDecoderPipe::Dispose()
{
	_packet_queue.Dispose();
	_decoder_pipe->Dispose();
	_decode_thread_exit.Wait();
}

void video::ThreadDecoderPipe::InitDecodeThread()
{
	std::thread([this]()
	{
		try
		{
			DecodeThreadFunc();
		}
		catch (...)
		{

		}

		_decode_thread_exit.SetResult();
	}).detach();
	_decode_thread_exit.Reset();
}

void video::ThreadDecoderPipe::DecodeThreadFunc()
{
	AVPacketWrapper packet;
	while (true)
	{
		int read_packet_result = _packet_queue.ReadPacket(packet);
		switch (read_packet_result)
		{
		case 0:
			{
				_decoder_pipe->SendPacket(&packet);
				break;
			}
		case (int)ErrorCode::eof:
			{
				if (_do_not_flush_consumer)
				{
					_decoder_pipe->FlushDecoderButNotFlushConsumers();
				}
				else
				{
					_decoder_pipe->SendPacket(nullptr);
				}

				return;
			}
		default:
			{
				throw std::runtime_error(ToString((ErrorCode)read_packet_result));
			}
		}
	}
}

void video::ThreadDecoderPipe::SendPacket(AVPacketWrapper *packet)
{
	if (packet == nullptr)
	{
		_do_not_flush_consumer = false;
	}

	_packet_queue.SendPacket(packet);
}

void video::ThreadDecoderPipe::FlushDecoderButNotFlushConsumers()
{
	_do_not_flush_consumer = true;
	_packet_queue.SendPacket(nullptr);
}

AVRational video::ThreadDecoderPipe::TimeBase() const
{
	return _decoder_pipe->TimeBase();
}

void video::ThreadDecoderPipe::SetTimeBase(AVRational value)
{
	_decoder_pipe->SetTimeBase(value);
}

AVSampleFormat video::ThreadDecoderPipe::SampleFormat() const
{
	return _decoder_pipe->SampleFormat();
}

void video::ThreadDecoderPipe::SetSampleFormat(AVSampleFormat value)
{
	_decoder_pipe->SetSampleFormat(value);
}

int video::ThreadDecoderPipe::SampleRate() const
{
	return _decoder_pipe->SampleRate();
}

void video::ThreadDecoderPipe::SetSampleRate(int value)
{
	_decoder_pipe->SetSampleRate(value);
}

AVChannelLayout video::ThreadDecoderPipe::ChannelLayout() const
{
	return _decoder_pipe->ChannelLayout();
}

void video::ThreadDecoderPipe::SetChannelLayout(AVChannelLayout value)
{
	_decoder_pipe->SetChannelLayout(value);
}

int video::ThreadDecoderPipe::Width() const
{
	return _decoder_pipe->Width();
}

void video::ThreadDecoderPipe::SetWidth(int value)
{
	_decoder_pipe->SetWidth(value);
}

int video::ThreadDecoderPipe::Height() const
{
	return _decoder_pipe->Height();
}

void video::ThreadDecoderPipe::SetHeight(int value)
{
	_decoder_pipe->SetHeight(value);
}

AVPixelFormat video::ThreadDecoderPipe::PixelFormat() const
{
	return _decoder_pipe->PixelFormat();
}

void video::ThreadDecoderPipe::SetPixelFormat(AVPixelFormat value)
{
	_decoder_pipe->SetPixelFormat(value);
}

AVRational video::ThreadDecoderPipe::FrameRate() const
{
	return _decoder_pipe->FrameRate();
}

void video::ThreadDecoderPipe::SetFrameRate(AVRational value)
{
	_decoder_pipe->SetFrameRate(value);
}
