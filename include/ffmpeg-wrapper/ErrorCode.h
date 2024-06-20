#pragma once
#include<exception>
#include<ffmpeg-wrapper/base_include.h>
#include<iostream>
#include<jccpp/define.h>
#include<sstream>
#include<string>

using std::exception;
using std::string;
using std::stringstream;
using std::endl;

namespace video
{
	enum class ErrorCode
	{
		unknow,// 此错误暂时没有被此枚举类型纳入
		eof = AVERROR_EOF,//到达文件尾

		/// <summary>
		///		输出暂时不可用。需要先输入才能产生更多的输出数据。
		/// </summary>
		output_is_temporarily_unavailable = AVERROR(EAGAIN),

		/// <summary>
		///		产生于编解码器
		///		1. 当前编码器未打开，或者它是一个解码器
		///		2. 当前解码器未打开，或者它是一个编码器。或者此解码器需要冲洗
		/// </summary>
		encoder_not_opened = AVERROR(EINVAL),
	};

	/// <summary>
	///		将错误代码转为字符串。
	/// </summary>
	/// <param name="error_code">
	///		错误代码。
	/// 
	///		* 不一定要是 ErrorCode 枚举类里面列出来的值，也可以是 ffmpeg 返回的 int 类型的错误代码，
	///		因为 ErrorCode 没有列出所有错误代码。当然，为了调用到本 ToString 重载，得将 int 强制
	///		转换为 ErrorCode。
	/// </param>
	/// <returns></returns>
	std::string ToString(ErrorCode error_code);
}