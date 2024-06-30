#pragma once
#include <base/pipe/ISource.h>
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>

namespace video
{
	using IPacketSource = base::ISource<AVPacketWrapper>;
}
