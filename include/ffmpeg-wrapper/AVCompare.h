#pragma once
#include <ffmpeg-wrapper/ffmpeg_headers.h>

bool operator==(AVRational const a, AVRational const b);
bool operator==(AVChannelLayout const ch1, AVChannelLayout const ch2);
