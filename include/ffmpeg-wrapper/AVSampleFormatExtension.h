#pragma once
#include <ffmpeg-wrapper/base_include.h>
#include <string>

namespace video
{
	class AVSampleFormatExtension
	{
	private:
		AVSampleFormatExtension() = delete;

	public:
		/// <summary>
		///		将 AVSampleFormat 转为字符串。与 av_sample_format_to_string 的不同之处
		///		在于，本函数返回的字符串末尾会拼接上 le 或 be。在大端模式下拼接 be，
		///		在小端模式下拼接 le
		/// </summary>
		/// <param name="sample_format">
		///		要被转化为字符串的 AVSampleFormat
		/// </param>
		/// <param name="little_endian">是否是小端模式</param>
		/// <returns>返回 AVSampleFormat 的字符串</returns>
		static std::string av_sample_format_to_string_with_endian(AVSampleFormat sample_format, bool little_endian = true);

		/// <summary>
		///		将字符串转为 AVSampleFormat
		/// </summary>
		/// <param name="str"></param>
		/// <returns>
		///		返回 AVSampleFormat。
		///		如果无法解析字符串，返回 AVSampleFormat::AV_SAMPLE_FMT_NONE。
		/// </returns>
		static AVSampleFormat string_to_av_sample_format(std::string str);

		/// <summary>
		///		分析特定编码器需要多少个采样点。
		/// </summary>
		/// <param name="codec_name"></param>
		/// <returns></returns>
		static int ParseRequiredSampleCount(std::string codec_name);
	};
}

/// @brief 将 AVSampleFormat 变成字符串输出
/// @param ostream
/// @param sample_format
/// @return
std::ostream &operator<<(std::ostream &ostream, AVSampleFormat sample_format);
