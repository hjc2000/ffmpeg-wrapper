#pragma once
#include "ffmpeg-wrapper/ffmpeg.h"
#include <iostream>
#include <string>

namespace base
{
	std::string ToString(AVRational const &value);
	std::string ToString(AVSampleFormat const &value);
	std::string ToString(AVPixelFormat const &value);
	std::string ToString(AVMediaType const &value);
} // namespace base

std::ostream &operator<<(std::ostream &ostream, AVMediaType const &right);
std::ostream &operator<<(std::ostream &ostream, AVRational const &right);
std::ostream &operator<<(std::ostream &ostream, AVPixelFormat const &right);
