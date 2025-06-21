#pragma once
#include "ffmpeg-wrapper/ffmpeg.h"
#include <string>

namespace video
{
	/// @brief 提供声道的位掩码
	class AudioChannelBitMark
	{
	private:
		AudioChannelBitMark() = delete;

	public:
		static consteval uint64_t Bit1()
		{
			return 1;
		}

		static consteval uint64_t Left()
		{
			return Bit1() << AVChannel::AV_CHAN_FRONT_LEFT;
		}

		static consteval uint64_t Right()
		{
			return Bit1() << AVChannel::AV_CHAN_FRONT_RIGHT;
		}

		static consteval uint64_t FrontCenter()
		{
			return Bit1() << AVChannel::AV_CHAN_FRONT_CENTER;
		}
	};

	class AVChannelLayoutExtension
	{
	private:
		AVChannelLayoutExtension() = delete;

	public:
		/// @brief 获取声道布局的描述字符串
		/// @param layout
		/// @return
		static std::string ChannelLayoutDescription(AVChannelLayout const &layout);

		/// @brief 获取指定声道数的默认声道布局。
		/// @param nb_channels 声道的数量。
		/// @return 声道布局。
		static AVChannelLayout GetDefaultChannelLayout(int nb_channels)
		{
			AVChannelLayout ch_layout;
			av_channel_layout_default(&ch_layout, nb_channels);
			return ch_layout;
		}
	};
} // namespace video

/// @brief 输出 AVChannelLayout 的描述信息字符串
/// @param ostream
/// @param ch_layout
/// @return
std::ostream &operator<<(std::ostream &ostream, AVChannelLayout ch_layout);
