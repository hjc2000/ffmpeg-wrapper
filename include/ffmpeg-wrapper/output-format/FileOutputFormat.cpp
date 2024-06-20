#include"FileOutputFormat.h"
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>

using namespace video;

video::FileOutputFormat::FileOutputFormat(std::string url)
{
	int ret = avformat_alloc_output_context2(
		&_wrapped_obj,
		nullptr,
		nullptr,
		url.c_str()
	);

	if (ret < 0)
	{
		throw std::runtime_error{ ToString((ErrorCode)ret) };
	}

	ret = avio_open(&_wrapped_obj->pb, url.c_str(), AVIO_FLAG_WRITE);
	if (ret < 0)
	{
		// 抛出异常前需要清理已经分配的资源。构造函数抛出异常，析构函数不会被自动调用。
		avformat_free_context(_wrapped_obj);
		throw std::runtime_error{ ToString((ErrorCode)ret) };
	}
}

video::FileOutputFormat::~FileOutputFormat()
{
	avio_closep(&_wrapped_obj->pb);
	avformat_free_context(_wrapped_obj);
}
