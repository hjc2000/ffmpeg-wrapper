#include"DecoderPipeFactory.h"
#include<DecoderPipe.h>

using namespace std;
using namespace video;

std::shared_ptr<DecoderPipeFactory> DecoderPipeFactory::Instance()
{
	static std::shared_ptr<DecoderPipeFactory> o { new DecoderPipeFactory { } };
	return o;
}

shared_ptr<IDecoderPipe> DecoderPipeFactory::CreateDecoderPipe(AVStreamInfoCollection const &infos)
{
	return shared_ptr<IDecoderPipe>{new DecoderPipe { infos }};
}
