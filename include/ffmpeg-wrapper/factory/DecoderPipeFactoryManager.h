#pragma once
#include <ffmpeg-wrapper/factory/IDecoderPipeFactory.h>

namespace video
{
	class DecoderPipeFactoryManager
	{
	public:
		static std::shared_ptr<video::IDecoderPipeFactory> Factory();
		static void SetCustomFactory(std::shared_ptr<video::IDecoderPipeFactory> o);
		static std::shared_ptr<video::IDecoderPipeFactory> DefaultFactory();
	};
} // namespace video
