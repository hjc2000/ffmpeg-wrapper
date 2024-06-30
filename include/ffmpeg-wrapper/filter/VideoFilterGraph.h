#pragma once
#include <base/pipe/ISource.h>
#include <ffmpeg-wrapper/info-collection/VideoStreamInfoCollection.h>
#include <ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include <ffmpeg-wrapper/wrapper/AVDictionaryWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVFilterContextWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>

namespace video
{
	/// @brief 视频滤镜图
	class VideoFilterGraph
		: public base::Wrapper<AVFilterGraph>,
		  public IFrameConsumer,
		  public base::ISource<AVFrameWrapper>
	{
	private:
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

		/// @brief 获取本滤镜图的起点
		/// @return
		AVFilterContextWrapper &buffer_filter();

		/// @brief 获取本滤镜图的终点
		/// @return
		AVFilterContextWrapper &buffer_sink_filter();

		/// @brief 构造一个 fps 滤镜
		/// @param FrameRate
		/// @return
		AVFilterContextWrapper alloc_fps_filter(AVRational FrameRate);

		void config_graph();

		int ReadData(AVFrameWrapper &frame) override;
		void SendData(AVFrameWrapper *frame) override;
	};
}
