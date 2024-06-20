#include<ffmpeg-wrapper/AVToString.h>
#include<ffmpeg-wrapper/base_include.h>

std::string ToString(AVRational value)
{
	return std::format("{}/{}", value.num, value.den);
}

std::string ToString(AVSampleFormat sample_format)
{
	return av_get_sample_fmt_name(sample_format);
}

std::string ToString(AVPixelFormat pixel_format)
{
	const char *name = ::av_get_pix_fmt_name(pixel_format);
	if (name == nullptr)
	{
		throw std::invalid_argument{ CODE_POS_STR + std::string{"未知的像素格式"} };
	}

	return name;
}

std::string ToString(AVMediaType media_type)
{
	return std::string{ ::av_get_media_type_string(media_type) };
}

std::ostream &operator<<(std::ostream &ostream, AVMediaType const media_type)
{
	return ostream << ToString(media_type);
}

std::ostream &operator<<(std::ostream &ostream, AVRational const rational)
{
	return ostream << ToString(rational);
}

std::ostream &operator<<(std::ostream &ostream, AVPixelFormat pixel_format)
{
	return ostream << ToString(pixel_format);
}
