#include "DecoderPipeFactoryManager.h"
#include <ffmpeg-wrapper/pipe/DecoderPipe.h>

std::shared_ptr<video::IDecoderPipeFactory> video::DecoderPipeFactoryManager::DefaultFactory()
{
	class DecoderPipeFactory
		: public video::IDecoderPipeFactory
	{
	public:
		std::shared_ptr<video::IDecoderPipe> CreateDecoderPipe(video::AVStreamInfoCollection const &infos) override
		{
			return std::shared_ptr<video::IDecoderPipe>{new video::DecoderPipe{infos}};
		}
	};

	static std::shared_ptr<video::IDecoderPipeFactory> o{new DecoderPipeFactory{}};
	return o;
}
