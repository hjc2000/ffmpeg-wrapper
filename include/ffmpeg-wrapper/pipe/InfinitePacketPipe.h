#pragma once
#include<ffmpeg-wrapper/AVCalculate.h>
#include<ffmpeg-wrapper/AVCompare.h>
#include<ffmpeg-wrapper/AVToString.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPipePacketSource.h>

namespace video
{
	class InfinitePacketPipe :
		public IPacketConsumer,
		public IPipePacketSource
	{
		int64_t _last_pts = 0;
		int64_t _last_dts = 0;
		int64_t _offset = 0;
		int64_t _last_packet_duration = 0;
		base::List<shared_ptr<IPacketConsumer>> _consumer_list;

		void UpdateLastPacketDuration(int64_t value);
		void UpdateLastPts(int64_t value);
		void UpdateLastDts(int64_t value);

		#pragma region 校准起始时间戳
		bool _start_pts_dts_not_set = true;
		int64_t _start_pts = 0;
		int64_t _start_dts = 0;
		int64_t _correct_offset = 0;

		/// <summary>
		///		如果有一个文件的第一个包的时间戳就是很大的值，则取 pts 和 dts
		///		的最小值，让 pts 和 dts 减去这个最小值。这样可让时间戳起始点最接近 0.
		/// </summary>
		/// <param name="packet"></param>
		void CorrectStartTimeStamp(AVPacketWrapper &packet);
		#pragma endregion

	public:
		base::List<shared_ptr<IPacketConsumer>> &PacketConsumerList() override
		{
			return _consumer_list;
		}

		/// <summary>
		///		送入包。会在调整包的 pts 和 dts 后送给消费者。
		/// 
		///		如果送入空指针，不会将空指针送给消费者，空指针在这里的作用是表示当前文件的包读完了，
		///		将开始下一个文件。下一个包会发生时间戳的回溯。这时候需要更新内部记录的偏移量，
		///		等新文件的包开始后，需要将时间戳加上偏移量。所以本函数无法冲洗消费者。
		///		冲洗消费者需要调用 FlushConsumer 函数。
		/// 
		///		送进来的包的时间基必须是 1/90000，否则会抛出异常。
		/// </summary>
		/// <param name="packet"></param>
		void SendPacket(AVPacketWrapper *packet) override;

		void FlushConsumer();
	};
}