#include "DecoderPipeFactoryManager.h"
#include <ffmpeg-wrapper/pipe/DecoderPipe.h>

using namespace video;

class DecoderPipeFactory
	: public video::IDecoderPipeFactory
{
public:
	std::shared_ptr<video::IDecoderPipe> CreateDecoderPipe(video::AVStreamInfoCollection const &infos) override
	{
		return std::shared_ptr<video::IDecoderPipe>{new video::DecoderPipe{infos}};
	}
};

std::shared_ptr<video::IDecoderPipeFactory> video::DecoderPipeFactoryManager::DefaultFactory()
{
	static std::shared_ptr<video::IDecoderPipeFactory> o{new DecoderPipeFactory{}};
	return o;
}
