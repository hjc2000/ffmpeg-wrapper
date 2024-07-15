#include "DecoderPipeFactoryManager.h"
#include <atomic>
#include <base/container/List.h>
#include <base/pipe/IConsumer.h>
#include <base/task/TaskCompletionSignal.h>
#include <ffmpeg-wrapper/AVToString.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include <ffmpeg-wrapper/pipe/interface/IDecoderPipe.h>
#include <ffmpeg-wrapper/wrapper/AVCodecContextWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include <memory>
#include <vector>

using namespace video;

class DecoderPipe final
	: public IDecoderPipe
{
private:
	AVStreamInfoCollection _stream_infos;
	std::atomic_bool _disposed = false;
	std::shared_ptr<AVCodecContextWrapper> _decoder;
	AVFrameWrapper _decoder_out_frame;
	base::List<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> _consumer_list;

	void read_and_send_frame()
	{
		while (!_disposed)
		{
			int ret = _decoder->ReadFrame(_decoder_out_frame);
			switch (ret)
			{
			case 0:
			{
				SendDataToEachConsumer(_decoder_out_frame);
				break;
			}
			case (int)ErrorCode::output_is_temporarily_unavailable:
			{
				// 解码器需要继续输入包才能输出帧
				return;
			}
			case (int)ErrorCode::eof:
			{
				// 解码器已被冲洗
				FlushEachConsumer();
				return;
			}
			default:
			{
				throw std::runtime_error{ToString((ErrorCode)ret)};
			}
			}
		}
	}

public:
	DecoderPipe(AVStreamInfoCollection stream)
	{
		_stream_infos = stream;
		_decoder = AVCodecContextWrapper::CreateDecoder(stream);
	}

	~DecoderPipe()
	{
		Dispose();
	}

	/// @brief
	void Dispose() override
	{
		if (_disposed)
		{
			return;
		}

		_disposed = true;
	}

	base::IList<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> &ConsumerList() override
	{
		return _consumer_list;
	}

	/// <summary>
	///		送入包。会解码，然后将帧送给消费者。
	///		如果没有任何消费者，本函数会直接返回，不会真正去解码，这样可以节省性能。
	///		如果送入的包的流索引和初始化本对象时的流索引不匹配，会直接返回，不会执行解码。
	/// </summary>
	/// <param name="packet"></param>
	void SendData(AVPacketWrapper &packet) override
	{
		if (ConsumerList().Count() == 0)
		{
			// 管道出口没有接收者。直接返回，节省性能。
			return;
		}

		// 防止解码器中有数据残留，会导致送入包失败
		read_and_send_frame();

		// packet 不是空指针
		if (packet.StreamIndex() != _stream_infos._index)
		{
			// 索引不匹配，直接返回。
			return;
		}

		_decoder->SendPacket(&packet);
		read_and_send_frame();
	}

	void Flush() override
	{
		// 冲洗解码器
		_decoder->SendPacket(nullptr);
		read_and_send_frame();
	}

	void FlushDecoderButNotFlushConsumers() override
	{
		_decoder->SendPacket(nullptr);
		while (!_disposed)
		{
			int ret = _decoder->ReadFrame(_decoder_out_frame);
			switch (ret)
			{
			case 0:
			{
				SendDataToEachConsumer(_decoder_out_frame);
				break;
			}
			case (int)ErrorCode::output_is_temporarily_unavailable:
			{
				// 解码器需要继续输入包才能输出帧
				return;
			}
			case (int)ErrorCode::eof:
			{
				// 解码器已被冲洗，但这里不冲洗消费者。
				return;
			}
			default:
			{
				throw std::runtime_error{ToString((ErrorCode)ret)};
			}
			}
		}
	}

#pragma region 通过 IAudioStreamInfoCollection 继承
	AVRational TimeBase() const override
	{
		return _stream_infos._timebase;
	}
	void SetTimeBase(AVRational value) override
	{
		_stream_infos._timebase = value;
	}

	AVSampleFormat SampleFormat() const override
	{
		return _stream_infos._sample_format;
	}
	void SetSampleFormat(AVSampleFormat value) override
	{
		_stream_infos._sample_format = value;
	}

	int SampleRate() const override
	{
		return _stream_infos._sample_rate;
	}
	void SetSampleRate(int value) override
	{
		_stream_infos._sample_rate = value;
	}

	AVChannelLayout ChannelLayout() const override
	{
		return _stream_infos._ch_layout;
	}
	void SetChannelLayout(AVChannelLayout value) override
	{
		_stream_infos._ch_layout = value;
	}
#pragma endregion

#pragma region 通过 IVideoStreamInfoCollection 继承
	int Width() const override
	{
		return _stream_infos._width;
	}
	void SetWidth(int value) override
	{
		_stream_infos._width = value;
	}

	int Height() const override
	{
		return _stream_infos._height;
	}
	void SetHeight(int value) override
	{
		_stream_infos._height = value;
	}

	AVPixelFormat PixelFormat() const override
	{
		return _stream_infos._pixel_format;
	}
	void SetPixelFormat(AVPixelFormat value) override
	{
		_stream_infos._pixel_format = value;
	}

	AVRational FrameRate() const override
	{
		return _stream_infos._frame_rate;
	}
	void SetFrameRate(AVRational value) override
	{
		_stream_infos._frame_rate = value;
	}
#pragma endregion
};

class DecoderPipeFactory
	: public video::IDecoderPipeFactory
{
public:
	std::shared_ptr<video::IDecoderPipe> CreateDecoderPipe(video::AVStreamInfoCollection const &infos) override
	{
		return std::shared_ptr<video::IDecoderPipe>{new DecoderPipe{infos}};
	}
};

std::shared_ptr<video::IDecoderPipeFactory> video::DecoderPipeFactoryManager::DefaultFactory()
{
	static std::shared_ptr<video::IDecoderPipeFactory> o{new DecoderPipeFactory{}};
	return o;
}
