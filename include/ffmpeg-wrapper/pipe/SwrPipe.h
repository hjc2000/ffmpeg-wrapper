#pragma once
#include<ffmpeg-wrapper/pipe/interface/IPipeFrameSource.h>
#include<ffmpeg-wrapper/wrapper/SwrContextWrapper.h>

namespace video
{
	class SwrPipe :
		public IPipeFrameSource,
		public IFrameConsumer
	{
		shared_ptr<SwrContextWrapper> _swr;
		AudioStreamInfoCollection _in_stream_infos;
		AudioFrameInfoCollection _desired_out_frame_infos;
		AVFrameWrapper _swr_out_frame;
		base::List<shared_ptr<IFrameConsumer>> _consumer_list;

		/// <summary>
		///		从 _swr 中读取帧，送给消费者。
		/// </summary>
		void read_and_send_frame();

		/// <summary>
		///		这个函数是给 change_swr 函数用的。
		///		读取并向消费者送入帧。但是遇到 eof 不会冲洗消费者。因为消费者的读取还没完，等会换个重采样器
		///		还要继续向它们送入帧。
		/// </summary>
		void read_and_send_frame_without_flushing_consumer();

		/// <summary>
		///		输入帧的参数与 _in_stream_infos 不同。流参数发生变化了，重新构造重采样器。
		/// </summary>
		void change_swr();

	public:
		/// <summary>
		///		重采样管道。
		/// </summary>
		/// <param name="desired_out_frame_infos">期望输出的音频帧是什么样的</param>
		SwrPipe(IAudioFrameInfoCollection &desired_out_frame_infos);

		base::List<shared_ptr<IFrameConsumer>> &FrameConsumerList() override
		{
			return _consumer_list;
		}

		/// <summary>
		///		向管道送入帧。重采样后会送给消费者。如果出口处没有任何消费者，则本函数会直接返回，不执行工作。
		/// </summary>
		/// <param name="frame"></param>
		void SendFrame(AVFrameWrapper *frame) override;
	};
}