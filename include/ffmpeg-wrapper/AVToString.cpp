#include <base/string/define.h>
#include <ffmpeg-wrapper/AVToString.h>
#include <ffmpeg-wrapper/base_include.h>
#include <format>

std::string base::ToString(AVRational value)
{
    return std::format("{}/{}", value.num, value.den);
}

std::string base::ToString(AVSampleFormat sample_format)
{
    return av_get_sample_fmt_name(sample_format);
}

std::string base::ToString(AVPixelFormat pixel_format)
{
    char const *name = ::av_get_pix_fmt_name(pixel_format);
    if (name == nullptr)
    {
        throw std::invalid_argument{CODE_POS_STR + std::string{"未知的像素格式"}};
    }

    return name;
}

std::string base::ToString(AVMediaType media_type)
{
    return std::string{::av_get_media_type_string(media_type)};
}

std::ostream &operator<<(std::ostream &ostream, AVMediaType const media_type)
{
    return ostream << base::ToString(media_type);
}

std::ostream &operator<<(std::ostream &ostream, AVRational const rational)
{
    return ostream << base::ToString(rational);
}

std::ostream &operator<<(std::ostream &ostream, AVPixelFormat pixel_format)
{
    return ostream << base::ToString(pixel_format);
}
