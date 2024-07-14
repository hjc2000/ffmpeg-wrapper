#include "ffmpeg-wrapper/pipe/InfinitePacketPipe.h"
#include <base/string/define.h>

using namespace video;
using namespace std;

void InfinitePacketPipe::UpdateLastPacketDuration(int64_t value)
{
	if (value > _last_packet_duration)
	{
		_last_packet_duration = value;
	}
}

void InfinitePacketPipe::UpdateLastDts(int64_t value)
{
	if (value > _last_dts)
	{
		_last_dts = value;
	}
}

void InfinitePacketPipe::CorrectStartingPointOfDts(AVPacketWrapper &packet)
{
	if (_first_dts_not_set)
	{
		_first_dts_not_set = false;
		_first_dts = packet.Dts();
	}

	if (packet.Pts() != AV_NOPTS_VALUE)
	{
		packet.SetPts(packet.Pts() - _first_dts);
	}

	packet.SetDts(packet.Dts() - _first_dts);
}

void InfinitePacketPipe::SendData(AVPacketWrapper &packet)
{
	// packet 不为空指针
	if (packet.TimeBase() != AVRational{1, 90000})
	{
		cout << CODE_POS_STR << "送进来的包的时间基必须是 " << AVRational{1, 90000} << endl;
	}

	CorrectStartingPointOfDts(packet);
	UpdateLastDts(packet.Dts());
	UpdateLastPacketDuration(packet.Duration());

	if (packet.Pts() != AV_NOPTS_VALUE)
	{
		packet.SetPts(packet.Pts() + _offset);
	}

	packet.SetDts(packet.Dts() + _offset);
	SendDataToEachConsumer(packet);
}

void InfinitePacketPipe::Flush()
{
	_offset += _last_dts + _last_packet_duration;
	_last_dts = 0;
	_last_packet_duration = 0;
	_first_dts_not_set = true;

	FlushEachConsumer();
}
