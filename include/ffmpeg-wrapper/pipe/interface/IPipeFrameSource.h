#pragma once
#include <base/container/List.h>
#include <base/pipe/IConsumer.h>
#include <base/pipe/IPipeSource.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <memory>

namespace video
{
	using IPipeFrameSource = base::IPipeSource<AVFrameWrapper>;
}
