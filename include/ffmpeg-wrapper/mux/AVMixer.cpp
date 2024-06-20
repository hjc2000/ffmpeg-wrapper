#include"AVMixer.h"

using namespace std;
using namespace video;

void video::AVMixer::CreateNewVideoStream()
{
	_input_video_format->DumpFormat();
	AVStreamWrapper stream = _input_video_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
	if (stream.IsNull())
	{
		throw std::runtime_error { "没有视频流" };
	}

	_src_video_stream_index = stream.Index();
	AVStreamWrapper new_stream = _out_format->CreateNewStream();
	new_stream.SetCodecParams(stream.CodecParams());
	new_stream.SetIndex(0);
	new_stream.SetTimeBase(AVRational { 1, 90000 });
}

void video::AVMixer::CreateNewAudioStream()
{
	_input_audio_format->DumpFormat();
	AVStreamWrapper stream = _input_audio_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
	if (stream.IsNull())
	{
		throw std::runtime_error { "没有音频流" };
	}

	_src_audio_stream_index = stream.Index();
	AVStreamWrapper new_stream = _out_format->CreateNewStream();
	new_stream.SetCodecParams(stream.CodecParams());
	new_stream.SetIndex(1);
	new_stream.SetTimeBase(AVRational { 1, 90000 });
}

bool video::AVMixer::ReadVideoPacketOnce(shared_ptr<base::CancellationToken> cancel_pump)
{
	while (!cancel_pump->IsCancellationRequested())
	{
		int result = _input_video_format->ReadPacket(_temp_packet);
		switch (result)
		{
		case 0:
			{
				if (_temp_packet.StreamIndex() != _src_video_stream_index)
				{
					continue;
				}

				_temp_packet.SetStreamIndex(0);
				_temp_packet.ChangeTimeBase(AVRational { 1, 90000 });
				_video_time = _temp_packet.dts();
				_out_format->SendPacket(&_temp_packet);
				if (_video_time < _audio_time)
				{
					continue;
				}

				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	return false;
}

bool video::AVMixer::ReadAudioPacketOnce(shared_ptr<base::CancellationToken> cancel_pump)
{
	while (!cancel_pump->IsCancellationRequested())
	{
		int result = _input_audio_format->ReadPacket(_temp_packet);
		switch (result)
		{
		case 0:
			{
				if (_temp_packet.StreamIndex() != _src_audio_stream_index)
				{
					continue;
				}

				_temp_packet.SetStreamIndex(1);
				_temp_packet.ChangeTimeBase(AVRational { 1, 90000 });
				_audio_time = _temp_packet.dts();
				_out_format->SendPacket(&_temp_packet);
				if (_audio_time < _video_time)
				{
					continue;
				}

				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	return false;
}

video::AVMixer::AVMixer(
	shared_ptr<InputFormat> input_video_format,
	shared_ptr<InputFormat> input_audio_format,
	shared_ptr<OutputFormat> out_format
)
{
	_input_video_format = input_video_format;
	_input_audio_format = input_audio_format;
	_out_format = out_format;

	CreateNewVideoStream();
	CreateNewAudioStream();
}

void video::AVMixer::Pump(shared_ptr<base::CancellationToken> cancel_pump)
{
	_out_format->WriteHeader();
	_out_format->DumpFormat();
	while (!cancel_pump->IsCancellationRequested())
	{
		bool read_video_packet_result = ReadVideoPacketOnce(cancel_pump);
		bool read_audio_packet_result = ReadAudioPacketOnce(cancel_pump);
		if (!read_video_packet_result && !read_audio_packet_result)
		{
			_out_format->SendPacket(nullptr);
			_out_format->SendPacket(nullptr);
			return;
		}
	}
}
