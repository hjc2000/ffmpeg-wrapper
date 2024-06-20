#pragma once
#include<ffmpeg-wrapper/base_include.h>

namespace video
{
	/**
	 * @brief 提供声道的位掩码
	*/
	class AudioChannelBitMark
	{
	public:
		const static uint64_t Bit1 = 1;
		const static uint64_t Left = Bit1 << AVChannel::AV_CHAN_FRONT_LEFT;
		const static uint64_t Right = Bit1 << AVChannel::AV_CHAN_FRONT_RIGHT;
		const static uint64_t FrontCenter = Bit1 << AVChannel::AV_CHAN_FRONT_CENTER;
	};

	class AVChannelLayoutExtension
	{
	private:
		/// <summary>
		/// 作为静态类，禁止构造
		/// </summary>
		AVChannelLayoutExtension() {}

	public:
		/// <summary>
		/// 获取声道布局的描述字符串
		/// </summary>
		/// <param name="layout"></param>
		/// <returns></returns>
		static string channel_layout_description(AVChannelLayout layout);

		/// <summary>
		/// 获取单声道的声道布局
		/// </summary>
		/// <returns></returns>
		static AVChannelLayout get_mono_layout_channel();

		/// <summary>
		/// 获取呈 2.0 声道布局（左右声道，低音不分离）的 AVChannelLayout
		/// </summary>
		/// <returns></returns>
		static AVChannelLayout get_2_0_layout_channel();

		/// <summary>
		/// 获取 3.0 声道布局的 AVChannelLayout
		/// </summary>
		/// <returns></returns>
		static AVChannelLayout get_3_0_layout_channel();

		static AVChannelLayout GetDefaultChannelLayout(int nb_channels)
		{
			AVChannelLayout ch_layout;
			av_channel_layout_default(&ch_layout, nb_channels);
			return ch_layout;
		}
	};
}

/**
 * @brief 输出 AVChannelLayout 的描述信息字符串
 * @param ostream
 * @param ch_layout
 * @return
*/
std::ostream &operator<<(std::ostream &ostream, AVChannelLayout ch_layout);
