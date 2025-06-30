#include "ffmpeg-wrapper/filter/VideoFilterGraph.h"
#include "ffmpeg-wrapper/wrapper/AVDictionaryWrapper.h"
#include "VideoFilterGraph.h"

video::VideoFilterGraph::VideoFilterGraph(IVideoStreamInfoCollection const &infos)
{
	_input_video_stream_infos = infos;
	_wrapped_obj = avfilter_graph_alloc();
	if (_wrapped_obj == nullptr)
	{
		throw std::runtime_error{"avfilter_graph_alloc 失败。"};
	}

	init_buffer_filter();
	init_buffer_sink_filter();
}

void video::VideoFilterGraph::init_buffer_filter()
{
	AVFilter const *buffersrc = avfilter_get_by_name("buffer");

	AVFilterContext *buffersrc_ctx = avfilter_graph_alloc_filter(_wrapped_obj,
																 buffersrc,
																 "src");

	if (!buffersrc_ctx)
	{
		throw std::runtime_error{"avfilter_graph_alloc_filter 失败。"};
	}

	av_opt_set_int(buffersrc_ctx,
				   "width",
				   _input_video_stream_infos.Width(),
				   AV_OPT_SEARCH_CHILDREN);

	av_opt_set_int(buffersrc_ctx,
				   "height",
				   _input_video_stream_infos.Height(),
				   AV_OPT_SEARCH_CHILDREN);

	av_opt_set_pixel_fmt(buffersrc_ctx,
						 "pix_fmt",
						 _input_video_stream_infos.PixelFormat(),
						 AV_OPT_SEARCH_CHILDREN);

	av_opt_set_q(buffersrc_ctx,
				 "time_base",
				 _input_video_stream_infos.TimeBase(),
				 AV_OPT_SEARCH_CHILDREN);

	av_opt_set_q(buffersrc_ctx,
				 "FrameRate",
				 _input_video_stream_infos.FrameRate(),
				 AV_OPT_SEARCH_CHILDREN);

	int ret = avfilter_init_str(buffersrc_ctx, nullptr);
	if (ret < 0)
	{
		throw std::runtime_error{base::ToString((ErrorCode)ret)};
	}

	_buffer_filter = AVFilterContextWrapper{buffersrc_ctx};
}

void video::VideoFilterGraph::init_buffer_sink_filter()
{
	AVFilter const *buffersink = avfilter_get_by_name("buffersink");

	AVFilterContext *buffersink_ctx = avfilter_graph_alloc_filter(_wrapped_obj,
																  buffersink,
																  "sink");

	if (buffersink_ctx == nullptr)
	{
		throw std::runtime_error{"AVFilterContext 构造失败"};
	}

	if (avfilter_init_str(buffersink_ctx, nullptr) < 0)
	{
		throw std::runtime_error{"AVFilterContext 初始化失败"};
	}

	_buffer_sink_filter = AVFilterContextWrapper{buffersink_ctx};
}

video::AVFilterContextWrapper &video::VideoFilterGraph::buffer_filter()
{
	return _buffer_filter;
}

video::AVFilterContextWrapper &video::VideoFilterGraph::buffer_sink_filter()
{
	return _buffer_sink_filter;
}

video::AVFilterContextWrapper video::VideoFilterGraph::alloc_fps_filter(AVRational FrameRate)
{
	AVFilter const *fps_filter = avfilter_get_by_name("fps");
	AVFilterContext *fps_ctx = avfilter_graph_alloc_filter(_wrapped_obj, fps_filter, "fps");
	if (fps_ctx == nullptr)
	{
		throw std::runtime_error{"AVFilterContext 构造失败"};
	}

	AVDictionaryWrapper options;
	options.SetValueByKey("fps", base::ToString(FrameRate).c_str());
	if (avfilter_init_dict(fps_ctx, &options.WrappedObj()) < 0)
	{
		throw std::runtime_error{"AVFilterContext 初始化失败"};
	}

	return AVFilterContextWrapper{fps_ctx};
}

void video::VideoFilterGraph::config_graph()
{
	int ret = avfilter_graph_config(_wrapped_obj, nullptr);
	if (ret < 0)
	{
		throw std::runtime_error{base::ToString((ErrorCode)ret)};
	}
}

bool video::VideoFilterGraph::ReadData(AVFrameWrapper &frame)
{
	int result = av_buffersink_get_frame(_buffer_sink_filter, frame);
	if (result < 0)
	{
		return false;
	}

	return true;
}

void video::VideoFilterGraph::SendData(AVFrameWrapper &frame)
{
	// 非冲洗模式
	int ret = av_buffersrc_add_frame_flags(_buffer_filter,
										   frame,
										   AV_BUFFERSRC_FLAG_KEEP_REF);

	if (ret < 0)
	{
		throw std::runtime_error{base::ToString((ErrorCode)ret)};
	}
}

void video::VideoFilterGraph::Flush()
{
	// 冲洗模式
	int ret = av_buffersrc_add_frame_flags(_buffer_filter,
										   nullptr,
										   AV_BUFFERSRC_FLAG_KEEP_REF);

	if (ret < 0)
	{
		throw std::runtime_error{base::ToString((ErrorCode)ret)};
	}
}
