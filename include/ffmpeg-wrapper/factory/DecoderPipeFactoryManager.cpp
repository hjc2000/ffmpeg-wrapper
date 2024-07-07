#include "DecoderPipeFactoryManager.h"
#include <ffmpeg-wrapper/pipe/DecoderPipe.h>

static std::shared_ptr<video::IDecoderPipeFactory> _costom_factory = nullptr;

std::shared_ptr<video::IDecoderPipeFactory> video::DecoderPipeFactoryManager::Factory()
{
	if (_costom_factory)
	{
		return _costom_factory;
	}

	return DefaultFactory();
}

void video::DecoderPipeFactoryManager::SetCustomFactory(std::shared_ptr<video::IDecoderPipeFactory> o)
{
	_costom_factory = o;
}

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
