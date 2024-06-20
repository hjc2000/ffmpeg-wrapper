#include"ffmpeg-wrapper/wrapper/AVPacketWrapper.h"

using namespace video;
using namespace std;

AVPacketWrapper::AVPacketWrapper()
{
	_wrapped_obj = av_packet_alloc();
	if (_wrapped_obj == nullptr)
	{
		throw std::runtime_error{ CODE_POS_STR + std::string{"构造 AVPacket 失败"} };
	}
}

AVPacketWrapper::AVPacketWrapper(AVPacketWrapper const &another) :AVPacketWrapper()
{
	ref(another);
}

AVPacketWrapper::~AVPacketWrapper()
{
	av_packet_free(&_wrapped_obj);
}

AVPacketWrapper &AVPacketWrapper::operator=(AVPacketWrapper const &another)
{
	ref(another);
	return *this;
}

void AVPacketWrapper::ChangeTimeBase(AVRational new_time_base)
{
	AVRational old_time_base = _wrapped_obj->time_base;
	_wrapped_obj->time_base = new_time_base;
	_wrapped_obj->pts = ConvertTimeStamp(_wrapped_obj->pts, old_time_base, new_time_base);
	_wrapped_obj->dts = ConvertTimeStamp(_wrapped_obj->dts, old_time_base, new_time_base);
	_wrapped_obj->duration = ConvertTimeStamp(_wrapped_obj->duration, old_time_base, new_time_base);
}

void AVPacketWrapper::ref(const AVPacketWrapper &other)
{
	unref();
	int ret = av_packet_ref(_wrapped_obj, other._wrapped_obj);
	if (ret < 0)
	{
		throw std::runtime_error{ CODE_POS_STR + std::string{"引用 AVPacket 失败"} };
	}
}

void AVPacketWrapper::unref()
{
	av_packet_unref(_wrapped_obj);
}

int AVPacketWrapper::StreamIndex() const
{
	return _wrapped_obj->stream_index;
}

void AVPacketWrapper::SetStreamIndex(int value)
{
	_wrapped_obj->stream_index = value;
}

int64_t AVPacketWrapper::Duration() const
{
	return _wrapped_obj->duration;
}

void AVPacketWrapper::SetDuration(int64_t value)
{
	_wrapped_obj->duration = value;
}

int64_t AVPacketWrapper::pts() const
{
	return _wrapped_obj->pts;
}

void AVPacketWrapper::set_pts(int64_t value)
{
	_wrapped_obj->pts = value;
}

int64_t AVPacketWrapper::dts() const
{
	return _wrapped_obj->dts;
}

void AVPacketWrapper::set_dts(int64_t value)
{
	_wrapped_obj->dts = value;
}

AVRational AVPacketWrapper::TimeBase() const
{
	return _wrapped_obj->time_base;
}

void AVPacketWrapper::SetTimeBase(AVRational value)
{
	_wrapped_obj->time_base = value;
}
