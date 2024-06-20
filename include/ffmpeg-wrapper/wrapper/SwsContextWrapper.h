#pragma once
#include<atomic>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/info-collection/VideoFrameInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameSource.h>
#include<base/Wrapper.h>
#include<mutex>

namespace video
{
	class SwsContextWrapper :
		public base::Wrapper<SwsContext>,
		public IFrameConsumer,
		public IFrameSource
	{
		SwsContext *_wrapped_obj = nullptr;
		VideoFrameInfoCollection _in_video_frame_infos;
		VideoFrameInfoCollection _out_video_frame_infos;
		AVFrameWrapper _in_frame;

		/**
		* @brief _out_frame 的数据可用，还没被取走。
		*/
		bool _out_frame_avaliable = false;
		bool _flushed = false;
		std::mutex _lock;

	public:
		SwsContextWrapper(
			IVideoFrameInfoCollection &in_video_frame_infos,
			IVideoFrameInfoCollection &out_video_frame_infos
		);

		~SwsContextWrapper()
		{
			sws_freeContext(_wrapped_obj);
			_wrapped_obj = nullptr;
		}

		SwsContext *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		SwsContext *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		/**
		 * @brief 送入帧后应该立刻调用 read_frame 读取，如果没有读取就继续送入，会抛出异常。
		 * @param frame 要送入的帧。可以送入空指针。虽然 sws 不需要冲洗，它的内部没有队列。
		*/
		void SendFrame(AVFrameWrapper *frame) override;

		/**
		 * @brief 读出帧。
		 * @param frame
		 * @return
		*/
		int ReadFrame(AVFrameWrapper &frame) override;
	};
}