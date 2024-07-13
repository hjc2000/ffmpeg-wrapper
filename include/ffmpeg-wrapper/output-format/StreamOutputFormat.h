#pragma once
#include <ffmpeg-wrapper/output-format/IOContextOutputFormat.h>

namespace video
{
	class StreamOutputFormat
		: public IOContextOutputFormat
	{
	public:
		/// @brief 构造函数
		/// @param url 这里的 url 不是用来创建文件的，而是让 ffmpeg 根据后缀名分析封装格式的。
		/// @param output_stream 封装的输出将被写入流中。
		StreamOutputFormat(std::string url, std::shared_ptr<base::Stream> output_stream);
	};
}