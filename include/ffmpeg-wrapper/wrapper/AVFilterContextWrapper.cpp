#include "ffmpeg-wrapper/wrapper/AVFilterContextWrapper.h"
#include <ffmpeg-wrapper/AVToString.h>
#include <ffmpeg-wrapper/ErrorCode.h>

video::AVFilterContextWrapper::AVFilterContextWrapper(AVFilterContext *o)
{
    _wrapped_obj = o;
}

video::AVFilterContextWrapper::AVFilterContextWrapper(AVFilterContextWrapper const &o)
{
    *this = o;
}

video::AVFilterContextWrapper &video::AVFilterContextWrapper::operator=(AVFilterContextWrapper const &o)
{
    _wrapped_obj = o._wrapped_obj;
    return *this;
}

void video::AVFilterContextWrapper::Link(AVFilterContextWrapper &next_filter)
{
    int ret = avfilter_link(_wrapped_obj, 0, next_filter, 0);
    if (ret)
    {
        throw std::runtime_error{base::ToString((ErrorCode)ret)};
    }
}

video::AVFilterContextWrapper &video::AVFilterContextWrapper::operator<<(AVFilterContextWrapper &next_filter)
{
    Link(next_filter);
    return next_filter;
}
