#include "AVSampleFormatExtension.h"
#include <base/string/define.h>
#include <ffmpeg-wrapper/AVToString.h>
#include <format>
#include <map>

using namespace video;

std::ostream &operator<<(std::ostream &ostream, AVSampleFormat sample_format)
{
    return ostream << av_get_sample_fmt_name(sample_format);
}

std::string video::AVSampleFormatExtension::av_sample_format_to_string_with_endian(AVSampleFormat sample_format, bool little_endian)
{
    std::string base = av_get_sample_fmt_name(sample_format);
    if (little_endian)
    {
        return base + "le";
    }
    else
    {
        return base + "be";
    }
}

AVSampleFormat video::AVSampleFormatExtension::string_to_av_sample_format(std::string str)
{
    return ::av_get_sample_fmt(str.c_str());
}

int video::AVSampleFormatExtension::ParseRequiredSampleCount(std::string codec_name)
{
    static std::map<std::string, int> sample_count_map = {
        {"aac", 1024},
        {"libmp3lame", 1152},
        {"ac3", 1536},
        {"eac3", 1536},
        {"libopus", 960},
        {"libvorbis", 1024},
        {"flac", 4096},
    };

    // 使用 map 查找来获取采样点数，如果找不到，返回默认值 1024
    auto it = sample_count_map.find(codec_name);
    if (it != sample_count_map.end())
    {
        return it->second;
    }

    std::cout << CODE_POS_STR
              << std::format("map 中没有指示 {} 应该有多少个采样点，返回默认的 1024 个", codec_name)
              << std::endl;

    return 1024;
}
