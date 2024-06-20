#pragma once
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/output-format/OutputFormat.h>
#include<functional>
#include<iostream>
#include<string>

namespace video
{
	class FileOutputFormat :public OutputFormat
	{
		AVFormatContext *_wrapped_obj;

	public:
		FileOutputFormat(std::string url);
		~FileOutputFormat();

		AVFormatContext *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		AVFormatContext *WrappedObj() const override
		{
			return _wrapped_obj;
		}
	};
}