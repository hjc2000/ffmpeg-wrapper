#pragma once
#include <base/pipe/IPump.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <ffmpeg-wrapper/input-format/InputFormat.h>
#include <ffmpeg-wrapper/output-format/OutputFormat.h>
#include <ffmpeg-wrapper/pipe/InfinitePacketPipe.h>
#include <memory>

namespace video
{
    /// @brief 接受一个音频输入格式，一个视频输入格式，将两个输入格式解封装，将它们的包混合在一起，
    /// 写入输出格式。
    /// @note 视频包的流索引会调整为 0，音频包的流索引会调整为 1.
    class AVMixer :
        base::IPump
    {
    private:
        std::shared_ptr<InputFormat> _input_video_format;
        std::shared_ptr<InputFormat> _input_audio_format;
        std::shared_ptr<OutputFormat> _out_format;
        AVPacketWrapper _temp_packet;
        int _src_video_stream_index = 0;
        int _src_audio_stream_index = 0;
        int64_t _audio_time = 0;
        int64_t _video_time = 0;

        void CreateNewVideoStream();
        void CreateNewAudioStream();
        bool ReadVideoPacketOnce(std::shared_ptr<base::CancellationToken> cancel_pump);
        bool ReadAudioPacketOnce(std::shared_ptr<base::CancellationToken> cancel_pump);

    public:
        AVMixer(std::shared_ptr<InputFormat> input_video_format,
                std::shared_ptr<InputFormat> input_audio_format,
                std::shared_ptr<OutputFormat> out_format);

        void PumpDataToConsumers(std::shared_ptr<base::CancellationToken> cancel_pump) override;
    };
} // namespace video
