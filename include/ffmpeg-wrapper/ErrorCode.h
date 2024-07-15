#pragma once
#include <base/string/define.h>
#include <exception>
#include <ffmpeg-wrapper/base_include.h>
#include <iostream>
#include <sstream>
#include <string>

namespace video
{
	/// @brief 错误代码枚举
	enum class ErrorCode
	{
		/// @brief 此错误暂时没有被此枚举类型纳入
		unknow,

		/// @brief 到达文件尾
		eof = AVERROR_EOF,

		/// @brief 输出暂时不可用。需要先输入才能产生更多的输出数据。
		output_is_temporarily_unavailable = AVERROR(EAGAIN),

		/// @brief 产生于编解码器
		/// @note 可能的情况
		///		@li 当前编码器未打开，或者它是一个解码器
		///		@li 当前解码器未打开，或者它是一个编码器。或者此解码器需要冲洗
		encoder_not_opened = AVERROR(EINVAL),
	};

	/// @brief 将错误代码转为字符串。
	///
	/// @param error_code 错误代码。
	/// @note 不一定要是 ErrorCode 枚举类里面列出来的值，也可以是 ffmpeg 返回的 int 类型的错误代码，
	///		  因为 ErrorCode 没有列出所有错误代码。当然，为了调用到本 ToString 重载，得将 int 强制
	///		  转换为 ErrorCode。
	///
	/// @return
	std::string ToString(ErrorCode error_code);
}
