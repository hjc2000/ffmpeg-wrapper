#include "AudioStreamInfoCollection.h"

video::AudioStreamInfoCollection::AudioStreamInfoCollection(AudioStreamInfoCollection const &o)
{
    IAudioStreamInfoCollection::operator=(o);
}

video::AudioStreamInfoCollection::AudioStreamInfoCollection(std::shared_ptr<IAudioStreamInfoCollection> o)
{
    if (o == nullptr)
    {
        throw std::invalid_argument{"禁止传入空指针"};
    }

    *this = *o;
}

video::AudioStreamInfoCollection::AudioStreamInfoCollection(IAudioStreamInfoCollection const &o)
{
    IAudioStreamInfoCollection::operator=(o);
}

video::AudioStreamInfoCollection &video::AudioStreamInfoCollection::operator=(AudioStreamInfoCollection const &o)
{
    IAudioStreamInfoCollection::operator=(o);
    return *this;
}
