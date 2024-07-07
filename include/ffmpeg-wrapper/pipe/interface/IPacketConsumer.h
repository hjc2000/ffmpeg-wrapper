#pragma once
#include <base/pipe/IConsumer.h>
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>

namespace video
{
	using IPacketConsumer = base::IConsumer<AVPacketWrapper>;
}
