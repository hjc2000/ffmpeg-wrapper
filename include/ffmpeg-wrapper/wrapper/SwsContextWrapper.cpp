#include "ffmpeg-wrapper/wrapper/SwsContextWrapper.h"
#include "SwsContextWrapper.h"
#include <ffmpeg-wrapper/wrapper/SwrContextWrapper.h>

using namespace video;

video::SwsContextWrapper::SwsContextWrapper(SwsContextWrapper_InVideoFrameInfos const &in_video_frame_infos,
											SwsContextWrapper_OutVideoFrameInfos const &out_video_frame_infos)
{
	_in_video_frame_infos = in_video_frame_infos.Value();
	_out_video_frame_infos = out_video_frame_infos.Value();

	_wrapped_obj = sws_getContext(_in_video_frame_infos.Width(),
								  _in_video_frame_infos.Height(),
								  _in_video_frame_infos.PixelFormat(),
								  _out_video_frame_infos.Width(),
								  _out_video_frame_infos.Height(),
								  _out_video_frame_infos.PixelFormat(),
								  SWS_FAST_BILINEAR, // 使用双线性快速滤波算法
								  nullptr,
								  nullptr,
								  nullptr);

	if (!_wrapped_obj)
	{
		throw std::runtime_error{CODE_POS_STR + std::string{"sws_getContext 失败"}};
	}
}

video::SwsContextWrapper::~SwsContextWrapper()
{
	sws_freeContext(_wrapped_obj);
	_wrapped_obj = nullptr;
}

void video::SwsContextWrapper::SendData(AVFrameWrapper &frame)
{
	if (_flushed)
	{
		throw std::runtime_error{CODE_POS_STR + std::string{"冲洗后不允许再送入"}};
	}

	if (_out_frame_avaliable)
	{
		throw std::runtime_error{CODE_POS_STR + std::string{"还没有读出帧却继续送入帧"}};
	}

	_in_frame = frame;
	_out_frame_avaliable = true;
}

void video::SwsContextWrapper::Flush()
{
	_flushed = true;
}

bool video::SwsContextWrapper::ReadData(AVFrameWrapper &frame)
{
	if (_flushed && !_out_frame_avaliable)
	{
		return false;
	}

	if (!_out_frame_avaliable)
	{
		return false;
	}

	_out_frame_avaliable = false;

	frame = AVFrameWrapper{_out_video_frame_infos};
	frame.SetTimeBase(_in_frame.TimeBase());
	frame.SetPts(_in_frame.Pts());

	sws_scale(_wrapped_obj,
			  _in_frame->data,
			  _in_frame->linesize,
			  0,
			  _in_frame->height,
			  frame->data,
			  frame->linesize);

	return true;
}
