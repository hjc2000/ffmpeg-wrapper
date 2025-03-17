#include "ffmpeg-wrapper/input-format/InputFormat.h"
#include <ffmpeg-wrapper/AVToString.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <ffmpeg-wrapper/pch.h>
#include <ffmpeg-wrapper/wrapper/AVDictionaryWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include <iostream>
#include <string>


using namespace video;

video::InputFormat::InputFormat(std::string url)
{
	_url = url;
	_wrapped_obj = avformat_alloc_context();
	int result = avformat_open_input(&_wrapped_obj, url.c_str(), nullptr, nullptr);
	if (result < 0)
	{
		throw std::runtime_error{CODE_POS_STR + std::string{"打开输入格式失败"}};
	}

	FindStreamInfo();
}

video::InputFormat::InputFormat(std::string url, AVInputFormat const *fmt, AVDictionary **options)
{
	_url = url;
	_wrapped_obj = avformat_alloc_context();
	int ret = avformat_open_input(&_wrapped_obj, url.c_str(), fmt, options);
	if (ret < 0)
	{
		throw std::runtime_error{CODE_POS_STR + std::string{"打开输入格式失败"}};
	}

	FindStreamInfo();
}

video::InputFormat::InputFormat(std::shared_ptr<AVIOContextWrapper> io_context)
{
	_url = "costom io context";
	_io_context = io_context;
	_wrapped_obj = avformat_alloc_context();
	_wrapped_obj->pb = *_io_context;
	_wrapped_obj->flags |= AVFMT_FLAG_CUSTOM_IO;
	int ret = avformat_open_input(&_wrapped_obj, nullptr, nullptr, nullptr);
	if (ret < 0)
	{
		throw std::runtime_error{CODE_POS_STR + std::string{"打开输入格式失败"}};
	}

	FindStreamInfo();
}

video::InputFormat::InputFormat(std::shared_ptr<base::Stream> input_stream)
	: InputFormat(std::shared_ptr<AVIOContextWrapper>{
		  new AVIOContextWrapper{
			  video::AVIOContextWrapper_IsWrite{false},
			  input_stream,
		  },
	  })
{
	_url = "costom stream";
}

InputFormat::~InputFormat()
{
	avformat_close_input(&_wrapped_obj);
	avformat_free_context(_wrapped_obj);
	_wrapped_obj = nullptr;
}

void InputFormat::DumpFormat()
{
	std::cout << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << "▼ 格式信息" << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;
	av_dump_format(_wrapped_obj, 0, _url.c_str(), false);
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << std::endl;
}

void InputFormat::FindStreamInfo(::AVDictionary **options)
{
	int ret = ::avformat_find_stream_info(_wrapped_obj, options);
	if (ret < 0)
	{
		throw std::runtime_error{CODE_POS_STR + std::string{"查找流信息失败"}};
	}
}

AVStreamWrapper InputFormat::FindBestStream(AVMediaType type)
{
	int result = av_find_best_stream(_wrapped_obj,
									 type,
									 -1,
									 -1,
									 nullptr,
									 0);

	if (result < 0)
	{
		std::cerr << CODE_POS_STR
				  << "找不到最好的 "
				  << base::ToString(type)
				  << " 流"
				  << std::endl;

		return AVStreamWrapper{nullptr};
	}

	return AVStreamWrapper{_wrapped_obj->streams[result]};
}

int InputFormat::ReadData(AVPacketWrapper &data)
{
	int ret = av_read_frame(_wrapped_obj, data);
	if (ret == 0)
	{
		// 读取成功
		data->time_base = _wrapped_obj->streams[data->stream_index]->time_base;
	}

	return ret;
}

std::chrono::seconds video::InputFormat::DurationInSeconds()
{
	return std::chrono::seconds{_wrapped_obj->duration / AV_TIME_BASE};
}

int InputFormat::StreamCount()
{
	return _wrapped_obj->nb_streams;
}

AVStreamWrapper InputFormat::GetStream(int stream_index)
{
	// 强制转换为无符号类型就不用判断 stream_index >= 0 了
	if ((uint32_t)stream_index >= _wrapped_obj->nb_streams)
	{
		throw std::runtime_error{CODE_POS_STR + std::string{"流索引号超出范围"}};
	}

	return AVStreamWrapper{_wrapped_obj->streams[stream_index]};
}
