#include "ffmpeg-wrapper/AVChannelLayoutExtension.h"

string video::AVChannelLayoutExtension::channel_layout_description(AVChannelLayout layout)
{
	char buf[64];
	av_channel_layout_describe(&layout, buf, sizeof(buf));
	return string(buf);
}

AVChannelLayout video::AVChannelLayoutExtension::get_mono_layout_channel()
{
	AVChannelLayout channel_layout;
	channel_layout.order = AVChannelOrder::AV_CHANNEL_ORDER_NATIVE;
	channel_layout.nb_channels = 1;
	channel_layout.u.mask = AudioChannelBitMark::Left;
	return channel_layout;
}

AVChannelLayout video::AVChannelLayoutExtension::get_2_0_layout_channel()
{
	AVChannelLayout channel_layout;
	channel_layout.order = AVChannelOrder::AV_CHANNEL_ORDER_NATIVE;
	channel_layout.nb_channels = 2;
	channel_layout.u.mask = AudioChannelBitMark::Left |
		AudioChannelBitMark::Right;

	return channel_layout;
}

AVChannelLayout video::AVChannelLayoutExtension::get_3_0_layout_channel()
{
	AVChannelLayout channel_layout;
	channel_layout.order = AVChannelOrder::AV_CHANNEL_ORDER_NATIVE;
	channel_layout.nb_channels = 3;
	channel_layout.u.mask = AudioChannelBitMark::Left |
		AudioChannelBitMark::Right |
		AudioChannelBitMark::FrontCenter;

	return channel_layout;
}

std::ostream &operator<<(std::ostream &ostream, AVChannelLayout ch_layout)
{
	ostream << video::AVChannelLayoutExtension::channel_layout_description(ch_layout);
	return ostream;
}
