#include "ffmpeg-wrapper/wrapper/SwsContextWrapper.h"
#include<ffmpeg-wrapper/wrapper/SwrContextWrapper.h>

using namespace video;

video::SwsContextWrapper::SwsContextWrapper(
	IVideoFrameInfoCollection &in_video_frame_infos,
	IVideoFrameInfoCollection &out_video_frame_infos
)
{
	_in_video_frame_infos = in_video_frame_infos;
	_out_video_frame_infos = out_video_frame_infos;

	_wrapped_obj = sws_getContext(
		in_video_frame_infos.Width(),
		in_video_frame_infos.Height(),
		in_video_frame_infos.PixelFormat(),
		out_video_frame_infos.Width(),
		out_video_frame_infos.Height(),
		out_video_frame_infos.PixelFormat(),
		SWS_FAST_BILINEAR,// 使用双线性快速滤波算法
		nullptr,
		nullptr,
		nullptr
	);

	if (!_wrapped_obj)
	{
		throw std::runtime_error{ CODE_POS_STR + std::string{"构造 sws 失败"} };
	}
}

void video::SwsContextWrapper::SendFrame(AVFrameWrapper *frame)
{
	std::lock_guard l(_lock);
	if (_flushed)
	{
		throw std::runtime_error{ CODE_POS_STR + std::string{"冲洗后不允许再送入"} };
	}

	if (!frame)
	{
		_flushed = true;
		return;
	}

	if (_out_frame_avaliable)
	{
		throw std::runtime_error{ CODE_POS_STR + std::string{"还没有读出帧却继续送入帧"} };
	}

	_in_frame = *frame;
	_out_frame_avaliable = true;
}

int video::SwsContextWrapper::ReadFrame(AVFrameWrapper &frame)
{
	std::lock_guard l(_lock);
	if (_flushed && !_out_frame_avaliable)
	{
		return (int)ErrorCode::eof;
	}

	if (!_out_frame_avaliable)
	{
		return (int)ErrorCode::output_is_temporarily_unavailable;
	}

	_out_frame_avaliable = false;

	frame = AVFrameWrapper{ _out_video_frame_infos };
	frame.SetTimeBase(_in_frame.TimeBase());
	frame.set_pts(_in_frame.pts());

	sws_scale(_wrapped_obj,
		_in_frame->data,
		_in_frame->linesize,
		0,
		_in_frame->height,
		frame->data,
		frame->linesize
	);

	_in_frame.unref();
	return 0;
}
