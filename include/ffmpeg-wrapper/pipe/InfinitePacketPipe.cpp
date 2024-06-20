#include"ffmpeg-wrapper/pipe/InfinitePacketPipe.h"

using namespace video;
using namespace std;

void InfinitePacketPipe::UpdateLastPacketDuration(int64_t value)
{
	if (value > _last_packet_duration)
	{
		_last_packet_duration = value;
	}
}

void InfinitePacketPipe::UpdateLastPts(int64_t value)
{
	if (value > _last_pts)
	{
		_last_pts = value;
	}
}

void InfinitePacketPipe::UpdateLastDts(int64_t value)
{
	if (value > _last_dts)
	{
		_last_dts = value;
	}
}

void InfinitePacketPipe::CorrectStartTimeStamp(AVPacketWrapper &packet)
{
	if (_start_pts_dts_not_set)
	{
		_start_pts_dts_not_set = false;
		_start_pts = packet.pts();
		_start_dts = packet.dts();

		// 开头处对齐取最小值，因为不能出现时间戳回溯。
		_correct_offset = std::min(_start_pts, _start_dts);
	}

	packet.set_pts(packet.pts() - _correct_offset);
	packet.set_dts(packet.dts() - _correct_offset);
}

void InfinitePacketPipe::SendPacket(AVPacketWrapper *packet)
{
	if (!packet)
	{
		// 结尾处添加边距取最大值，不能让下一个封装的时间戳重叠到上一个。
		_offset += std::max(_last_pts, _last_dts) + _last_packet_duration;
		_last_pts = 0;
		_last_dts = 0;
		_last_packet_duration = 0;
		_start_pts_dts_not_set = true;
		return;
	}

	// packet 不为空指针
	if (packet->TimeBase() != AVRational{ 1,90000 })
	{
		cout << CODE_POS_STR << "送进来的包的时间基必须是 " << AVRational{ 1,90000 } << endl;
	}

	CorrectStartTimeStamp(*packet);
	UpdateLastPts(packet->pts());
	UpdateLastDts(packet->dts());
	UpdateLastPacketDuration(packet->Duration());
	packet->set_pts(packet->pts() + _offset);
	packet->set_dts(packet->dts() + _offset);
	SendPacketToEachConsumer(packet);
}

void InfinitePacketPipe::FlushConsumer()
{
	SendPacketToEachConsumer(nullptr);
}
