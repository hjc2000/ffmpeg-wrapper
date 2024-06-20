#pragma once
#include<ffmpeg-wrapper/base_include.h>

int64_t operator*(int64_t const int_num, AVRational const rational);
int64_t operator*(AVRational const rational, int64_t const int_num);
AVRational operator*(AVRational r1, AVRational r2);

/**
 * @brief r1 / r2
 * @param r1 被除数
 * @param r2 除数
 * @return r1 / r2
*/
AVRational operator/(AVRational r1, AVRational r2);

/**
 * @brief 求倒数
 * @param rational
 * @return
*/
AVRational reciprocal(AVRational rational);

/// <summary>
///		将一个时间基的时间戳转换到另一个时间基。
/// </summary>
/// <param name="in_time_stamp"></param>
/// <param name="in_time_base"></param>
/// <param name="out_time_base"></param>
/// <returns>转换到 out_time_base 后的时间戳。</returns>
int64_t ConvertTimeStamp(int64_t in_time_stamp, AVRational in_time_base, AVRational out_time_base);
