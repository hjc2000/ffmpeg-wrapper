#pragma once
#include<ffmpeg-wrapper/output-format/OutputFormat.h>
#include<ffmpeg-wrapper/wrapper/AVIOContextWrapper.h>

namespace video
{
	class IOContextOutputFormat :public OutputFormat
	{
		shared_ptr<AVIOContextWrapper> _io_context;
		AVFormatContext *_wrapped_obj = nullptr;

	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="url">这里的 url 不是用来创建文件的，而是让 ffmpeg 根据后缀名分析封装格式的</param>
		/// <param name="io_context"></param>
		IOContextOutputFormat(std::string url, shared_ptr<AVIOContextWrapper> io_context);

		~IOContextOutputFormat();

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