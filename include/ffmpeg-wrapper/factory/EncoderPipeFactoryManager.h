#pragma once
#include <base/FactoryManager.h>
#include <ffmpeg-wrapper/factory/interface/IEncoderPipeFactory.h>

namespace video
{
    class EncoderPipeFactoryManager :
        public base::FactoryManager<video::IEncoderPipeFactory>
    {
    public:
        static EncoderPipeFactoryManager &Instance()
        {
            static EncoderPipeFactoryManager o;
            return o;
        }

        std::shared_ptr<video::IEncoderPipeFactory> DefaultFactory() override;
    };
} // namespace video
