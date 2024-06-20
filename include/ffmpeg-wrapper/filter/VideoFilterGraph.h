#pragma once
#include<ffmpeg-wrapper/info-collection/VideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameSource.h>
#include<ffmpeg-wrapper/wrapper/AVDictionaryWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVFilterContextWrapper.h>

namespace video
{
	class VideoFilterGraph :
		public base::Wrapper<AVFilterGraph>,
		public IFrameConsumer,
		public IFrameSource
	{
		AVFilterGraph *_wrapped_obj = nullptr;
		VideoStreamInfoCollection _input_video_stream_infos;
		AVFilterContextWrapper _buffer_filter;
		AVFilterContextWrapper _buffer_sink_filter;
		void init_buffer_filter();
		void init_buffer_sink_filter();

	public:
		VideoFilterGraph(IVideoStreamInfoCollection const &infos);

		AVFilterGraph *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		AVFilterGraph *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		/// <summary>
		///		获取本滤镜图的起点 —— buffer_filter
		/// </summary>
		/// <returns></returns>
		AVFilterContextWrapper &buffer_filter();

		/// <summary>
		///		获取本滤镜图的终点 —— buffer_sink_filter
		/// </summary>
		/// <returns></returns>
		AVFilterContextWrapper &buffer_sink_filter();

		/// <summary>
		///		构造一个 fps 滤镜
		/// </summary>
		/// <param name="FrameRate"></param>
		/// <returns></returns>
		AVFilterContextWrapper alloc_fps_filter(AVRational FrameRate);

		void config_graph();

		int ReadFrame(AVFrameWrapper &frame) override;
		void SendFrame(AVFrameWrapper *frame) override;
	};
}
