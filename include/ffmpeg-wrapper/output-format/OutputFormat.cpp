#include "OutputFormat.h"
#include <base/string/define.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

using namespace video;

void video::OutputFormat::WriteTrailer()
{
    int ret = av_write_trailer(_wrapped_obj);
    if (ret < 0)
    {
        throw std::runtime_error{base::ToString((ErrorCode)ret)};
    }
}

void video::OutputFormat::DumpFormat(char const *url)
{
    std::lock_guard l(_not_private_methods_lock);
    std::cout << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "▼ 格式信息" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
    av_dump_format(_wrapped_obj, 0, url, true);
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << std::endl;
}

bool video::OutputFormat::NeedGlobalHeader()
{
    std::lock_guard l(_not_private_methods_lock);
    return _wrapped_obj->oformat->flags & AVFMT_GLOBALHEADER;
}

AVStreamWrapper video::OutputFormat::CreateNewStream()
{
    std::lock_guard l(_not_private_methods_lock);
    ::AVStream *ps = avformat_new_stream(_wrapped_obj, nullptr);
    if (ps == nullptr)
    {
        throw std::runtime_error{"创建流失败"};
    }

    return AVStreamWrapper(ps);
}

AVStreamWrapper video::OutputFormat::CreateNewStream(std::shared_ptr<AVCodecContextWrapper> codec_ctx)
{
    std::lock_guard l{_not_private_methods_lock};
    AVStream *ps = avformat_new_stream(_wrapped_obj, nullptr);
    if (ps == nullptr)
    {
        throw std::runtime_error{"创建流失败"};
    }

    AVStreamWrapper stream{ps};

    /* SetCodecParam 函数设置参数的时候，会将编解码器的时间基，帧率的信息复制到流中。*/
    stream.SetCodecParams(*codec_ctx);
    return stream;
}

void video::OutputFormat::SendData(AVPacketWrapper &packet)
{
    std::lock_guard l(_not_private_methods_lock);
    int ret = av_interleaved_write_frame(_wrapped_obj, packet);
    if (ret < 0)
    {
        std::cout << CODE_POS_STR
                  << "错误代码："
                  << ret << " -- "
                  << base::ToString((ErrorCode)ret);
    }
}

void video::OutputFormat::Flush()
{
    std::lock_guard l(_not_private_methods_lock);
    _flush_times++;
    if (_flush_times == _wrapped_obj->nb_streams)
    {
        std::cout << CODE_POS_STR << "所有流都被冲洗了。" << std::endl;
        WriteTrailer();

        // 在后台线程中触发事件
        std::thread{
            [&]()
            {
                _all_streams_flushed_event.Invoke();
            }}
            .detach();
    }
}

void video::OutputFormat::WriteHeader(AVDictionary **dic)
{
    std::lock_guard l(_not_private_methods_lock);
    int ret = ::avformat_write_header(_wrapped_obj, dic);
    if (ret < 0)
    {
        throw std::runtime_error{base::ToString((ErrorCode)ret)};
    }
}
