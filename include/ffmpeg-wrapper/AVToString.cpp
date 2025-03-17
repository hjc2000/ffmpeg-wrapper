#include <base/string/define.h>
#include <ffmpeg-wrapper/AVToString.h>
#include <ffmpeg-wrapper/ffmpeg_headers.h>
#include <format>

std::string base::ToString(AVRational const &value)
{
	return std::format("{}/{}", value.num, value.den);
}

std::string base::ToString(AVSampleFormat const &value)
{
	return av_get_sample_fmt_name(value);
}

std::string base::ToString(AVPixelFormat const &value)
{
	char const *name = av_get_pix_fmt_name(value);
	if (name == nullptr)
	{
		throw std::invalid_argument{CODE_POS_STR + std::string{"未知的像素格式"}};
	}

	return name;
}

std::string base::ToString(AVMediaType const &value)
{
	return std::string{av_get_media_type_string(value)};
}

std::ostream &operator<<(std::ostream &ostream, AVMediaType const &right)
{
	return ostream << base::ToString(right);
}

std::ostream &operator<<(std::ostream &ostream, AVRational const &right)
{
	return ostream << base::ToString(right);
}

std::ostream &operator<<(std::ostream &ostream, AVPixelFormat const &right)
{
	return ostream << base::ToString(right);
}
