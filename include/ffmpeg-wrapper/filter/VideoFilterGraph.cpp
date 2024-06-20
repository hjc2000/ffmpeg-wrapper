#include"ffmpeg-wrapper/filter/VideoFilterGraph.h"

using namespace video;
using namespace std;

VideoFilterGraph::VideoFilterGraph(IVideoStreamInfoCollection const &infos) :
	_input_video_stream_infos(infos)
{
	_wrapped_obj = avfilter_graph_alloc();
	if (!_wrapped_obj)
	{
		throw std::runtime_error{ "avfilter_graph_alloc 失败。" };
	}

	init_buffer_filter();
	init_buffer_sink_filter();
}

void VideoFilterGraph::init_buffer_filter()
{
	const AVFilter *buffersrc = avfilter_get_by_name("buffer");
	AVFilterContext *buffersrc_ctx = avfilter_graph_alloc_filter(_wrapped_obj, buffersrc, "src");
	if (!buffersrc_ctx)
	{
		throw std::runtime_error{ "avfilter_graph_alloc_filter 失败。" };
	}

	av_opt_set_int(buffersrc_ctx, "width", _input_video_stream_infos.Width(), AV_OPT_SEARCH_CHILDREN);
	av_opt_set_int(buffersrc_ctx, "height", _input_video_stream_infos.Height(), AV_OPT_SEARCH_CHILDREN);
	av_opt_set_pixel_fmt(buffersrc_ctx, "pix_fmt", _input_video_stream_infos.PixelFormat(), AV_OPT_SEARCH_CHILDREN);
	av_opt_set_q(buffersrc_ctx, "time_base", _input_video_stream_infos.TimeBase(), AV_OPT_SEARCH_CHILDREN);
	av_opt_set_q(buffersrc_ctx, "FrameRate", _input_video_stream_infos.FrameRate(), AV_OPT_SEARCH_CHILDREN);
	int ret = avfilter_init_str(buffersrc_ctx, nullptr);
	if (ret < 0)
	{
		throw std::runtime_error{ ToString((ErrorCode)ret) };
	}

	_buffer_filter = AVFilterContextWrapper{ buffersrc_ctx };
}

void VideoFilterGraph::init_buffer_sink_filter()
{
	const AVFilter *buffersink = avfilter_get_by_name("buffersink");
	AVFilterContext *buffersink_ctx = avfilter_graph_alloc_filter(_wrapped_obj, buffersink, "sink");
	if (!buffersink_ctx)
	{
		throw std::runtime_error{ "AVFilterContext 构造失败" };
	}

	if (avfilter_init_str(buffersink_ctx, nullptr) < 0)
	{
		throw std::runtime_error{ "AVFilterContext 初始化失败" };
	}

	_buffer_sink_filter = AVFilterContextWrapper{ buffersink_ctx };
}

AVFilterContextWrapper &VideoFilterGraph::buffer_filter()
{
	return _buffer_filter;
}

AVFilterContextWrapper &VideoFilterGraph::buffer_sink_filter()
{
	return _buffer_sink_filter;
}

AVFilterContextWrapper VideoFilterGraph::alloc_fps_filter(AVRational FrameRate)
{
	const AVFilter *fps_filter = avfilter_get_by_name("fps");
	AVFilterContext *fps_ctx = avfilter_graph_alloc_filter(_wrapped_obj, fps_filter, "fps");
	if (!fps_ctx)
	{
		throw std::runtime_error{ "AVFilterContext 构造失败" };
	}

	AVDictionaryWrapper options;
	options.SetValueByKey("fps", ToString(FrameRate).c_str());
	if (avfilter_init_dict(fps_ctx, options) < 0)
	{
		throw std::runtime_error{ "AVFilterContext 初始化失败" };
	}

	return AVFilterContextWrapper{ fps_ctx };
}

void VideoFilterGraph::config_graph()
{
	int ret = avfilter_graph_config(_wrapped_obj, nullptr);
	if (ret < 0)
	{
		throw std::runtime_error{ ToString((ErrorCode)ret) };
	}
}

int VideoFilterGraph::ReadFrame(AVFrameWrapper &frame)
{
	frame.unref();
	return av_buffersink_get_frame(_buffer_sink_filter, frame);
}

void VideoFilterGraph::SendFrame(AVFrameWrapper *frame)
{
	if (!frame)
	{
		// 冲洗模式
		int ret = av_buffersrc_add_frame_flags(_buffer_filter, nullptr, AV_BUFFERSRC_FLAG_KEEP_REF);
		if (ret < 0)
		{
			throw std::runtime_error{ ToString((ErrorCode)ret) };
		}

		return;
	}

	// 非冲洗模式
	int ret = av_buffersrc_add_frame_flags(_buffer_filter, *frame, AV_BUFFERSRC_FLAG_KEEP_REF);
	if (ret < 0)
	{
		throw std::runtime_error{ ToString((ErrorCode)ret) };
	}
}
