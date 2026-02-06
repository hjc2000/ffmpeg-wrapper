#pragma once
#include "base/digital_circuit/MasterSlaveFlipFlop.h"
#include "ffmpeg-wrapper/ffmpeg.h"
#include <base/container/List.h>
#include <base/container/Queue.h>
#include <base/pipe/IPipeSource.h>
#include <base/Wrapper.h>
#include <ffmpeg-wrapper/AVCalculate.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <ffmpeg-wrapper/filter/VideoFilterGraph.h>
#include <memory>

namespace video
{
	/// @brief 用来调整帧率的管道。
	/// @note 帧从这里输入后再输出，帧率就变了。这里会插帧或删帧。
	class FpsAdjustPipe :
		public base::IConsumer<AVFrameWrapper>,
		public base::IPipeSource<AVFrameWrapper>
	{
	private:
		base::List<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> _consumer_list;
		VideoStreamInfoCollection _input_video_stream_infos;
		VideoFilterGraph _graph;
		AVRational _desired_out_fps;
		base::digital_circuit::MasterSlaveFlipFlop<int64_t> _trigger;

		void ReadAndSendFrame();

	public:
		/// @brief 构造函数
		/// @param input_video_stream_infos 输入的视频流信息
		/// @param desired_out_fps 期望的输出帧率
		FpsAdjustPipe(IVideoStreamInfoCollection const &input_video_stream_infos,
					  AVRational const &desired_out_fps);

		base::IList<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> &ConsumerList() override
		{
			return _consumer_list;
		}

		/// @brief 向 fps 调整管道送入帧。
		/// @note 处理完后帧会穿过管道，送给消费者。
		/// @note 离开管道的帧的时间基为 1/fps。此外，输出帧的 pts 会与输入帧进行同步，防止
		/// 输入端的 pts 跳跃后输出端仍然保持 pts 的连续，这将会导致音视频不同步。
		/// @note 如果输出端没有任何消费者，则本函数会直接返回，不执行任何工作。
		///
		/// @param frame
		void SendData(AVFrameWrapper &frame) override;

		void Flush() override;
	};
} // namespace video
