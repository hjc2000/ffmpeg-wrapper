#include "ffmpeg-wrapper/output-format/OutputFormat.h"
#include<ffmpeg-wrapper/ErrorCode.h>

using namespace video;

void video::OutputFormat::SendPacket(AVPacketWrapper *packet)
{
	std::lock_guard l(_not_private_methods_lock);
	if (!packet)
	{
		_flush_times++;
		if (_flush_times == WrappedObj()->nb_streams)
		{
			cout << CODE_POS_STR << "所有流都被冲洗了。" << endl;
			WriteTrailer();
			if (_on_all_stream_flushed_async)
			{
				std::thread([&]()
				{
					_on_all_stream_flushed_async();
				}).detach();
			}
		}

		return;
	}

	int ret = av_interleaved_write_frame(WrappedObj(), *packet);
	if (ret < 0)
	{
		cout << CODE_POS_STR << "错误代码：" << ret << " -- " << ToString((ErrorCode)ret);
	}
}

void video::OutputFormat::WriteHeader(AVDictionary **dic)
{
	std::lock_guard l(_not_private_methods_lock);
	int ret = ::avformat_write_header(WrappedObj(), dic);
	if (ret < 0)
	{
		throw std::runtime_error{ ToString((ErrorCode)ret) };
	}
}

void video::OutputFormat::WriteTrailer()
{
	int ret = ::av_write_trailer(WrappedObj());
	if (ret < 0)
	{
		throw std::runtime_error{ ToString((ErrorCode)ret) };
	}
}

bool video::OutputFormat::NeedGlobalHeader()
{
	std::lock_guard l(_not_private_methods_lock);
	return WrappedObj()->oformat->flags & AVFMT_GLOBALHEADER;
}

void video::OutputFormat::DumpFormat(char const *url)
{
	std::lock_guard l(_not_private_methods_lock);
	cout << endl;
	cout << "------------------------------------------------------------" << endl;
	cout << "▼ 格式信息" << endl;
	cout << "------------------------------------------------------------" << endl;
	av_dump_format(WrappedObj(), 0, url, true);
	cout << "------------------------------------------------------------" << endl;
	cout << endl;
}

AVStreamWrapper video::OutputFormat::CreateNewStream()
{
	std::lock_guard l(_not_private_methods_lock);
	::AVStream *ps = avformat_new_stream(WrappedObj(), nullptr);
	if (ps == nullptr)
	{
		throw std::runtime_error{ "创建流失败" };
	}

	return AVStreamWrapper(ps);
}

AVStreamWrapper video::OutputFormat::CreateNewStream(shared_ptr<AVCodecContextWrapper> codec_ctx)
{
	std::lock_guard l(_not_private_methods_lock);
	AVStream *ps = avformat_new_stream(WrappedObj(), nullptr);
	if (ps == nullptr)
	{
		throw std::runtime_error{ "创建流失败" };
	}

	AVStreamWrapper stream{ ps };

	/* SetCodecParam 函数设置参数的时候，会将码器的时间基，帧率的信息复制到流中。*/
	int ret = stream.SetCodecParams(codec_ctx);
	if (ret < 0)
	{
		throw std::runtime_error{ "设置流参数失败" };
	}

	return stream;
}
