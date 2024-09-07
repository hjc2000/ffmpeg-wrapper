#pragma once
#include <base/di/FactoryManager.h>
#include <ffmpeg-wrapper/factory/interface/IDecoderPipeFactory.h>

namespace video
{
    class DecoderPipeFactoryManager :
        public base::FactoryManager<video::IDecoderPipeFactory>
    {
    private:
        DecoderPipeFactoryManager() = default;

    public:
        static DecoderPipeFactoryManager &Instance()
        {
            static DecoderPipeFactoryManager o;
            return o;
        }

        std::shared_ptr<video::IDecoderPipeFactory> DefaultFactory() override;
    };
} // namespace video
