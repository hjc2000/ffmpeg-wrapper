#include "OutputFormat.h"
#include <ffmpeg-wrapper/ErrorCode.h>
#include <iostream>
#include <jccpp/define.h>
#include <memory>
#include <mutex>
#include <thread>

using namespace video;

void video::OutputFormat::SendPacket(AVPacketWrapper *packet)
{
	std::lock_guard l(_not_private_methods_lock);
	if (!packet)
	{
		_flush_times++;
		if (_flush_times == WrappedObj()->nb_streams)
		{
			std::cout << CODE_POS_STR << "所有流都被冲洗了。" << std::endl;
			WriteTrailer();
			if (_on_all_stream_flushed_async)
			{
				auto thread_func = [&]()
				{
					_on_all_stream_flushed_async();
				};
				std::thread(thread_func).detach();
			}
		}

		return;
	}

	int ret = av_interleaved_write_frame(WrappedObj(), *packet);
	if (ret < 0)
	{
		std::cout << CODE_POS_STR
				  << "错误代码："
				  << ret << " -- "
				  << ToString((ErrorCode)ret);
	}
}

void video::OutputFormat::WriteHeader(AVDictionary **dic)
{
	std::lock_guard l(_not_private_methods_lock);
	int ret = ::avformat_write_header(WrappedObj(), dic);
	if (ret < 0)
	{
		throw std::runtime_error{ToString((ErrorCode)ret)};
	}
}

void video::OutputFormat::WriteTrailer()
{
	int ret = ::av_write_trailer(WrappedObj());
	if (ret < 0)
	{
		throw std::runtime_error{ToString((ErrorCode)ret)};
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
	std::cout << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << "▼ 格式信息" << endl;
	std::cout << "------------------------------------------------------------" << std::endl;
	av_dump_format(WrappedObj(), 0, url, true);
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << std::endl;
}

AVStreamWrapper video::OutputFormat::CreateNewStream()
{
	std::lock_guard l(_not_private_methods_lock);
	::AVStream *ps = avformat_new_stream(WrappedObj(), nullptr);
	if (ps == nullptr)
	{
		throw std::runtime_error{"创建流失败"};
	}

	return AVStreamWrapper(ps);
}

AVStreamWrapper video::OutputFormat::CreateNewStream(std::shared_ptr<AVCodecContextWrapper> codec_ctx)
{
	std::lock_guard l(_not_private_methods_lock);
	AVStream *ps = avformat_new_stream(WrappedObj(), nullptr);
	if (ps == nullptr)
	{
		throw std::runtime_error{"创建流失败"};
	}

	AVStreamWrapper stream{ps};

	/* SetCodecParam 函数设置参数的时候，会将码器的时间基，帧率的信息复制到流中。*/
	int ret = stream.SetCodecParams(codec_ctx);
	if (ret < 0)
	{
		throw std::runtime_error{"设置流参数失败"};
	}

	return stream;
}
