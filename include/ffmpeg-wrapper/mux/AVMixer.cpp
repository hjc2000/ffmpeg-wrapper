#include "AVMixer.h"

using namespace video;

void video::AVMixer::CreateNewVideoStream()
{
	_input_video_format->DumpFormat();
	AVStreamWrapper stream = _input_video_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
	if (!stream)
	{
		throw std::runtime_error{"没有视频流"};
	}

	_src_video_stream_index = stream.Index();
	AVStreamWrapper new_stream = _out_format->CreateNewStream();
	new_stream.SetCodecParams(stream.CodecParams());
	new_stream.SetIndex(0);
	new_stream.SetTimeBase(AVRational{1, 90000});
}

void video::AVMixer::CreateNewAudioStream()
{
	_input_audio_format->DumpFormat();
	AVStreamWrapper stream = _input_audio_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
	if (!stream)
	{
		throw std::runtime_error{"没有音频流"};
	}

	_src_audio_stream_index = stream.Index();
	AVStreamWrapper new_stream = _out_format->CreateNewStream();
	new_stream.SetCodecParams(stream.CodecParams());
	new_stream.SetIndex(1);
	new_stream.SetTimeBase(AVRational{1, 90000});
}

bool video::AVMixer::ReadVideoPacketOnce(std::shared_ptr<base::CancellationToken> cancel_pump)
{
	while (true)
	{
		if (base::is_cancellation_requested(cancel_pump))
		{
			return false;
		}

		bool result = _input_video_format->ReadData(_temp_packet);
		if (!result)
		{
			return false;
		}

		// 读取成功
		if (_temp_packet.StreamIndex() != _src_video_stream_index)
		{
			continue;
		}

		_temp_packet.SetStreamIndex(0);
		_temp_packet.ChangeTimeBase(AVRational{1, 90000});
		_video_time = _temp_packet.Dts();
		_out_format->SendData(_temp_packet);
		if (_video_time < _audio_time)
		{
			// 让落后的视频追上音频
			continue;
		}

		return true;
	}
}

bool video::AVMixer::ReadAudioPacketOnce(std::shared_ptr<base::CancellationToken> cancel_pump)
{
	while (true)
	{
		if (base::is_cancellation_requested(cancel_pump))
		{
			return false;
		}

		bool result = _input_audio_format->ReadData(_temp_packet);
		if (!result)
		{
			return false;
		}

		if (_temp_packet.StreamIndex() != _src_audio_stream_index)
		{
			continue;
		}

		_temp_packet.SetStreamIndex(1);
		_temp_packet.ChangeTimeBase(AVRational{1, 90000});
		_audio_time = _temp_packet.Dts();
		_out_format->SendData(_temp_packet);
		if (_audio_time < _video_time)
		{
			// 让落后的音频追上视频
			continue;
		}

		return true;
	}
}

video::AVMixer::AVMixer(std::shared_ptr<InputFormat> input_video_format,
						std::shared_ptr<InputFormat> input_audio_format,
						std::shared_ptr<OutputFormat> out_format)
{
	if (input_video_format == nullptr)
	{
		throw std::invalid_argument{"input_video_format 不能是空指针"};
	}

	if (input_audio_format == nullptr)
	{
		throw std::invalid_argument{"input_audio_format 不能是空指针"};
	}

	if (out_format == nullptr)
	{
		throw std::invalid_argument{"out_format 不能是空指针"};
	}

	_input_video_format = input_video_format;
	_input_audio_format = input_audio_format;
	_out_format = out_format;

	CreateNewVideoStream();
	CreateNewAudioStream();
}

void video::AVMixer::PumpDataToConsumers(std::shared_ptr<base::CancellationToken> cancel_pump)
{
	_out_format->WriteHeader();
	_out_format->DumpFormat();

	while (true)
	{
		if (base::is_cancellation_requested(cancel_pump))
		{
			return;
		}

		bool read_video_packet_result = ReadVideoPacketOnce(cancel_pump);
		bool read_audio_packet_result = ReadAudioPacketOnce(cancel_pump);
		if (!read_video_packet_result && !read_audio_packet_result)
		{
			_out_format->Flush();
			_out_format->Flush();
			return;
		}
	}
}
