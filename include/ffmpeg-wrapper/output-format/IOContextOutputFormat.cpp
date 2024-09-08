#include "ffmpeg-wrapper/output-format/IOContextOutputFormat.h"
#include <ffmpeg-wrapper/ErrorCode.h>
#include <memory>

using namespace video;

IOContextOutputFormat::IOContextOutputFormat(std::string url,
                                             std::shared_ptr<AVIOContextWrapper> io_context)
{
    if (io_context == nullptr)
    {
        throw std::invalid_argument{"io_context 不能是空指针。"};
    }

    _io_context = io_context;
    int ret = avformat_alloc_output_context2(&_wrapped_obj,
                                             nullptr,
                                             nullptr,
                                             url.c_str());

    if (ret < 0)
    {
        throw std::runtime_error{base::ToString((ErrorCode)ret)};
    }

    _wrapped_obj->flags |= AVFMT_FLAG_CUSTOM_IO;
    _wrapped_obj->pb = *io_context;
}

IOContextOutputFormat::~IOContextOutputFormat()
{
    avformat_free_context(_wrapped_obj);
}
