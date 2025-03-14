#include "ffmpeg-wrapper/output-format/StreamOutputFormat.h"

using namespace video;

static std::shared_ptr<AVIOContextWrapper> CreateIOContext(std::shared_ptr<base::Stream> output_stream)
{
    std::shared_ptr<AVIOContextWrapper> output_context{
        new AVIOContextWrapper{
            video::AVIOContextWrapper_IsWrite{true},
            output_stream,
        },
    };

    return output_context;
}

StreamOutputFormat::StreamOutputFormat(std::string url, std::shared_ptr<base::Stream> output_stream)
    : IOContextOutputFormat(url, CreateIOContext(output_stream))
{
}
