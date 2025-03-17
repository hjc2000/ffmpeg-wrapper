#pragma once
#include <ffmpeg-wrapper/ffmpeg_headers.h>
#include <string>

namespace video
{
	/// @brief 分析字符串，转化为 AVMediaType
	/// @param str
	/// @return
	AVMediaType ParseAVMediaType(std::string str);
} // namespace video
