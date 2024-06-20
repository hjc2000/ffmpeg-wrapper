#include"ffmpeg-wrapper/input-format/InputFormat.h"
#include<ffmpeg-wrapper/AVToString.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/wrapper/AVDictionaryWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include<iostream>
#include<string>

using namespace video;
using namespace std;

video::InputFormat::InputFormat(std::string url)
{
	_url = url;
	_wrapped_obj = avformat_alloc_context();
	int ret = ::avformat_open_input(&_wrapped_obj, url.c_str(), nullptr, nullptr);
	if (ret < 0)
	{
		throw std::runtime_error { CODE_POS_STR + std::string { "打开输入格式失败" } };
	}

	FindStreamInfo();
}

video::InputFormat::InputFormat(std::string url, AVInputFormat const *fmt, AVDictionary **options)
{
	_url = url;
	_wrapped_obj = avformat_alloc_context();
	int ret = ::avformat_open_input(&_wrapped_obj, url.c_str(), fmt, options);
	if (ret < 0)
	{
		throw std::runtime_error { CODE_POS_STR + std::string { "打开输入格式失败" } };
	}

	FindStreamInfo();
}

video::InputFormat::InputFormat(shared_ptr<AVIOContextWrapper> io_context)
{
	_url = "costom io context";
	_io_context = io_context;
	_wrapped_obj = avformat_alloc_context();
	_wrapped_obj->pb = *_io_context;
	_wrapped_obj->flags |= AVFMT_FLAG_CUSTOM_IO;
	int ret = ::avformat_open_input(&_wrapped_obj, nullptr, nullptr, nullptr);
	if (ret < 0)
	{
		throw std::runtime_error { CODE_POS_STR + std::string { "打开输入格式失败" } };
	}

	FindStreamInfo();
}

video::InputFormat::InputFormat(shared_ptr<base::Stream> input_stream) :
	InputFormat(shared_ptr<AVIOContextWrapper>{ new AVIOContextWrapper { false, input_stream } })
{
	_url = "costom stream";
}

InputFormat::~InputFormat()
{
	::avformat_close_input(&_wrapped_obj);
	::avformat_free_context(_wrapped_obj);
	_wrapped_obj = nullptr;
}

void InputFormat::DumpFormat()
{
	cout << endl;
	cout << "------------------------------------------------------------" << endl;
	cout << "▼ 格式信息" << endl;
	cout << "------------------------------------------------------------" << endl;
	av_dump_format(_wrapped_obj, 0, _url.c_str(), false);
	cout << "------------------------------------------------------------" << endl;
	cout << endl;
}

void InputFormat::FindStreamInfo(::AVDictionary **options)
{
	int ret = ::avformat_find_stream_info(_wrapped_obj, options);
	if (ret < 0)
	{
		throw std::runtime_error { CODE_POS_STR + std::string { "查找流信息失败" } };
	}
}

AVStreamWrapper InputFormat::FindBestStream(AVMediaType type)
{
	int ret = ::av_find_best_stream(
		_wrapped_obj,
		type,
		-1,
		-1,
		nullptr,
		0
	);

	if (ret < 0)
	{
		cerr << CODE_POS_STR << "找不到最好的 " << ToString(type) << " 流" << endl;
		return AVStreamWrapper { nullptr };
	}

	return AVStreamWrapper { _wrapped_obj->streams[ret] };
}

int InputFormat::ReadPacket(AVPacketWrapper &packet)
{
	packet.unref();
	int ret = ::av_read_frame(_wrapped_obj, packet);
	if (!ret)
	{
		packet->time_base = _wrapped_obj->streams[packet->stream_index]->time_base;
	}

	return ret;
}

std::chrono::seconds video::InputFormat::DurationInSeconds()
{
	return std::chrono::seconds { _wrapped_obj->duration / AV_TIME_BASE };
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
		throw std::runtime_error { CODE_POS_STR + std::string { "流索引号超出范围" } };
	}

	return AVStreamWrapper { _wrapped_obj->streams[stream_index] };
}
