#pragma once
#include <ffmpeg-wrapper/ffmpeg_headers.h>
#include <ffmpeg-wrapper/output-format/OutputFormat.h>
#include <functional>
#include <iostream>
#include <string>

namespace video
{
	class FileOutputFormat :
		public OutputFormat
	{
	public:
		FileOutputFormat(std::string url);
		~FileOutputFormat();
	};
} // namespace video
