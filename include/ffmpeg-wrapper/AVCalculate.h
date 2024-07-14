#pragma once
#include <base/math/Fraction.h>
#include <ffmpeg-wrapper/base_include.h>

namespace video
{
	/// @brief 将 AVRational 转为 base::Fraction
	/// @param r
	/// @return
	base::Fraction AVRationalToFraction(AVRational const &r);

	/// @brief 将一个时间基的时间戳转换到另一个时间基。
	/// @param in_time_stamp
	/// @param in_time_base
	/// @param out_time_base
	/// @return 转换到 out_time_base 后的时间戳。
	int64_t ConvertTimeStamp(int64_t in_time_stamp, AVRational in_time_base, AVRational out_time_base);
}
