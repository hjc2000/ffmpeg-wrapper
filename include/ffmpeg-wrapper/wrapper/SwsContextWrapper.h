#pragma once
#include <atomic>
#include <base/pipe/IConsumer.h>
#include <base/pipe/ISource.h>
#include <base/Wrapper.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <ffmpeg-wrapper/ffmpeg_headers.h>
#include <ffmpeg-wrapper/info-collection/VideoFrameInfoCollection.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>

namespace video
{
#pragma region 参数类

	/// @brief 输入视频帧的信息
	class SwsContextWrapper_InVideoFrameInfos
	{
	private:
		VideoFrameInfoCollection _value;

	public:
		explicit SwsContextWrapper_InVideoFrameInfos(IVideoFrameInfoCollection const &value)
		{
			_value = value;
		}

		VideoFrameInfoCollection Value() const
		{
			return _value;
		}
	};

	/// @brief 输出视频帧的信息
	class SwsContextWrapper_OutVideoFrameInfos
	{
	private:
		VideoFrameInfoCollection _value;

	public:
		explicit SwsContextWrapper_OutVideoFrameInfos(IVideoFrameInfoCollection const &value)
		{
			_value = value;
		}

		VideoFrameInfoCollection Value() const
		{
			return _value;
		}
	};

#pragma endregion

	/// @brief 视频重采样器。
	class SwsContextWrapper :
		public base::Wrapper<SwsContext>,
		public base::IConsumer<AVFrameWrapper>,
		public base::ISource<AVFrameWrapper>
	{
	private:
		SwsContext *_wrapped_obj = nullptr;
		VideoFrameInfoCollection _in_video_frame_infos;
		VideoFrameInfoCollection _out_video_frame_infos;
		AVFrameWrapper _in_frame;

		/// @brief _out_frame 的数据可用，还没被取走。
		bool _out_frame_avaliable = false;
		bool _flushed = false;

	public:
		SwsContextWrapper(SwsContextWrapper_InVideoFrameInfos const &in_video_frame_infos,
						  SwsContextWrapper_OutVideoFrameInfos const &out_video_frame_infos);

		~SwsContextWrapper();

		SwsContext *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		/// @brief 送入帧后应该立刻调用 read_frame 读取，如果没有读取就继续送入，会抛出异常。
		/// @param frame 要送入的帧。可以送入空指针。虽然 sws 不需要冲洗，它的内部没有队列。
		void SendData(AVFrameWrapper &frame) override;

		/// @brief 不再 SendData 时需要冲洗。
		void Flush() override;

		/// @brief 读出帧。
		/// @param frame
		/// @return
		int ReadData(AVFrameWrapper &frame) override;
	};
} // namespace video
