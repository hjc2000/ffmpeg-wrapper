#pragma once
#include "base/math/Fraction.h"
#include "ffmpeg-wrapper/ffmpeg.h" // IWYU pragma: keep

namespace video
{
	/// @brief 将 AVRational 转为 base::Fraction
	/// @param r
	/// @return
	base::Fraction ToFraction(AVRational const &r);

} // namespace video
