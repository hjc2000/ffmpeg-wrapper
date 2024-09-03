#include "AVPacketWrapper.h"
#include <base/math/Fraction.h>
#include <base/string/define.h>

using namespace video;

#pragma region 私有生命周期

void AVPacketWrapper::Ref(AVPacketWrapper const &o)
{
    Unref();
    int ret = av_packet_ref(_wrapped_obj, o._wrapped_obj);
    if (ret < 0)
    {
        throw std::runtime_error{CODE_POS_STR + std::string{"引用 AVPacket 失败"}};
    }
}

void AVPacketWrapper::Unref()
{
    av_packet_unref(_wrapped_obj);
}

#pragma endregion

AVPacketWrapper::AVPacketWrapper()
{
    _wrapped_obj = av_packet_alloc();
    if (_wrapped_obj == nullptr)
    {
        throw std::runtime_error{CODE_POS_STR + std::string{"构造 AVPacket 失败"}};
    }
}

AVPacketWrapper::AVPacketWrapper(AVPacketWrapper const &o)
    : AVPacketWrapper()
{
    Ref(o);
}

AVPacketWrapper::~AVPacketWrapper()
{
    av_packet_free(&_wrapped_obj);
}

AVPacketWrapper &AVPacketWrapper::operator=(AVPacketWrapper const &o)
{
    Ref(o);
    return *this;
}

void AVPacketWrapper::ChangeTimeBase(AVRational new_time_base)
{
    base::Fraction fraction_old_time_base{
        _wrapped_obj->time_base.num,
        _wrapped_obj->time_base.den,
    };

    base::Fraction fraction_new_time_base{
        new_time_base.num,
        new_time_base.den,
    };

    _wrapped_obj->time_base = new_time_base;
    _wrapped_obj->pts = static_cast<int64_t>(_wrapped_obj->pts * fraction_old_time_base / fraction_new_time_base);
    _wrapped_obj->dts = static_cast<int64_t>(_wrapped_obj->dts * fraction_old_time_base / fraction_new_time_base);
    _wrapped_obj->duration = static_cast<int64_t>(_wrapped_obj->duration * fraction_old_time_base / fraction_new_time_base);
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

int64_t AVPacketWrapper::Pts() const
{
    return _wrapped_obj->pts;
}

void AVPacketWrapper::SetPts(int64_t value)
{
    _wrapped_obj->pts = value;
}

int64_t AVPacketWrapper::Dts() const
{
    return _wrapped_obj->dts;
}

void AVPacketWrapper::SetDts(int64_t value)
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
