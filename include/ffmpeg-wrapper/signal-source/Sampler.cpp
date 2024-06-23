#include "Sampler.h"

video::Sampler::Sampler(std::shared_ptr<base::ISignalSource<double, double>> signal_source)
{
	_signal_source = signal_source;
}

int video::Sampler::ReadFrame(AVFrameWrapper &frame)
{
	return 0;
}
