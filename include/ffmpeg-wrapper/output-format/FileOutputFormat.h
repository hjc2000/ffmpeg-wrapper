#pragma once
#include <ffmpeg-wrapper/output-format/OutputFormat.h>
#include <ffmpeg-wrapper/pch.h>
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
