#include "ffmpeg-wrapper/pipe/SwrPipe.h"
#include "SwrPipe.h"
#include <ffmpeg-wrapper/ErrorCode.h>

video::SwrPipe::SwrPipe(IAudioFrameInfoCollection &desired_out_frame_infos)
{
    // 先假设输入流和输出流是一样的，随后反正可以重新构造重采样器
    _in_stream_infos = desired_out_frame_infos;
    _desired_out_frame_infos = desired_out_frame_infos;
    _swr = std::shared_ptr<SwrContextWrapper>{
        new SwrContextWrapper{
            _in_stream_infos,
            _desired_out_frame_infos,
        },
    };
}

void video::SwrPipe::ReadAndSendFrame()
{
    while (1)
    {
        int ret = _swr->ReadData(_swr_out_frame);
        switch (ret)
        {
        case 0:
            {
                SendDataToEachConsumer(_swr_out_frame);

                // 下轮循环继续读取
                break;
            }
        case (int)ErrorCode::output_is_temporarily_unavailable:
            {
                return;
            }
        case (int)ErrorCode::eof:
            {
                FlushEachConsumer();
                return;
            }
        default:
            {
                throw std::runtime_error{base::ToString((ErrorCode)ret)};
            }
        }
    }
}

void video::SwrPipe::ReadAndSendFrameWithoutFlushingConsumer()
{
    while (1)
    {
        int ret = _swr->ReadData(_swr_out_frame);
        switch (ret)
        {
        case 0:
            {
                SendDataToEachConsumer(_swr_out_frame);

                // 下轮循环继续读取
                break;
            }
        case (int)ErrorCode::output_is_temporarily_unavailable:
            {
                return;
            }
        case (int)ErrorCode::eof:
            {
                return;
            }
        default:
            {
                throw std::runtime_error{base::ToString((ErrorCode)ret)};
            }
        }
    }
}

void video::SwrPipe::ReplaceSwr()
{
    std::cout << CODE_POS_STR << "重新构造 swr" << std::endl;

    // 冲洗旧的重采样器
    _swr->Flush();
    ReadAndSendFrameWithoutFlushingConsumer();

    // 构造新的重采样器
    _swr = std::shared_ptr<SwrContextWrapper>{
        new SwrContextWrapper{
            _in_stream_infos,
            _desired_out_frame_infos,
        },
    };
}

void video::SwrPipe::SendData(AVFrameWrapper &frame)
{
    if (ConsumerList().Count() == 0)
    {
        return;
    }

    ReadAndSendFrame();
    if (frame != _in_stream_infos)
    {
        _in_stream_infos = frame;
        ReplaceSwr();
    }

    _swr->SendData(frame);
    ReadAndSendFrame();
}

void video::SwrPipe::Flush()
{
    if (ConsumerList().Count() == 0)
    {
        return;
    }

    ReadAndSendFrame();
    _swr->Flush();
    ReadAndSendFrame();
}
