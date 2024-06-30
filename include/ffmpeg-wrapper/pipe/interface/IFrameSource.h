#pragma once
#include <base/pipe/ISource.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>

namespace video
{
	using IFrameSource = base::ISource<AVFrameWrapper>;
}
