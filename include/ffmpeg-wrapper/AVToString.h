#pragma once
#include<ffmpeg-wrapper/base_include.h>

std::string ToString(AVRational value);
std::string ToString(AVSampleFormat sample_format);
std::string ToString(AVPixelFormat pixel_format);
std::string ToString(AVMediaType media_type);

std::ostream &operator<<(std::ostream &ostream, AVMediaType const media_type);
std::ostream &operator<<(std::ostream &ostream, AVRational const rational);
std::ostream &operator<<(std::ostream &ostream, AVPixelFormat pf);
