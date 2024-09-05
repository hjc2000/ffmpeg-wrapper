#include "ThreadDecoderPipe.h"
#include <base/Guard.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <iostream>

using namespace video;

void video::ThreadDecoderPipe::InitDecodeThread()
{
    std::thread{
        [this]()
        {
            DecodeThreadFunc();
        }}
        .detach();

    _decode_thread_exit.Reset();
}

void video::ThreadDecoderPipe::DecodeThreadFunc()
{
    base::Guard g{
        [this]()
        {
            _decode_thread_exit.SetResult();
        },
    };

    try
    {
        AVPacketWrapper packet;
        while (true)
        {
            int read_packet_result = _packet_queue.ReadData(packet);
            if (read_packet_result < 0)
            {
                if (_do_not_flush_consumer)
                {
                    _decoder_pipe->FlushDecoderButNotFlushConsumers();
                }
                else
                {
                    _decoder_pipe->Flush();
                }

                return;
            }

            _decoder_pipe->SendData(packet);
        }
    }
    catch (std::exception const &e)
    {
        std::cout << CODE_POS_STR << "ThreadDecoderPipe 的线程遇到异常。" << e.what() << std::endl;
    }
}

video::ThreadDecoderPipe::ThreadDecoderPipe(AVStreamInfoCollection stream)
{
    _decoder_pipe = video::DecoderPipeFactoryManager::Instance().Factory()->CreateDecoderPipe(stream);
    InitDecodeThread();
}

video::ThreadDecoderPipe::~ThreadDecoderPipe()
{
    if (_disposed)
    {
        return;
    }

    _disposed = true;

    Dispose();
}

void video::ThreadDecoderPipe::Dispose()
{
    _packet_queue.Dispose();
    _decoder_pipe->Dispose();
    _decode_thread_exit.Wait();
}

void video::ThreadDecoderPipe::SendData(AVPacketWrapper &packet)
{
    _packet_queue.SendData(packet);
}

void video::ThreadDecoderPipe::Flush()
{
    _do_not_flush_consumer = false;
    _packet_queue.Flush();
}

void video::ThreadDecoderPipe::FlushDecoderButNotFlushConsumers()
{
    _do_not_flush_consumer = true;
    _packet_queue.Flush();
}

#pragma region IAudioStreamInfoCollection

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

#pragma endregion

#pragma region IVideoStreamInfoCollection

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

#pragma endregion
