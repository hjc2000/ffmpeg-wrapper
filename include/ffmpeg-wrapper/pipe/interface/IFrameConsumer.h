#pragma once
#include <base/pipe/IConsumer.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>

namespace video
{
	using IFrameConsumer = base::IConsumer<AVFrameWrapper *>;
}
