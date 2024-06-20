#include"ffmpeg-wrapper/wrapper/AVFilterContextWrapper.h"
#include<ffmpeg-wrapper/AVToString.h>
#include<ffmpeg-wrapper/ErrorCode.h>

using namespace video;
using namespace std;

AVFilterContextWrapper::AVFilterContextWrapper()
{

}

AVFilterContextWrapper::AVFilterContextWrapper(AVFilterContext *filter_ctx)
{
	_wrapped_obj = filter_ctx;
}

AVFilterContextWrapper::AVFilterContextWrapper(AVFilterContextWrapper const &other)
{
	_wrapped_obj = other._wrapped_obj;
}

AVFilterContextWrapper &AVFilterContextWrapper::operator=(AVFilterContextWrapper const &other)
{
	_wrapped_obj = other._wrapped_obj;
	return *this;
}

void AVFilterContextWrapper::link(AVFilterContextWrapper &next_filter)
{
	int ret = avfilter_link(_wrapped_obj, 0, next_filter, 0);
	if (ret)
	{
		throw std::runtime_error{ ToString((ErrorCode)ret) };
	}
}

AVFilterContextWrapper &AVFilterContextWrapper::operator<<(AVFilterContextWrapper &next_filter)
{
	link(next_filter);
	return next_filter;
}
