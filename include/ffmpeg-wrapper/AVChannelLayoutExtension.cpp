#include "AVChannelLayoutExtension.h"

std::string video::AVChannelLayoutExtension::ChannelLayoutDescription(AVChannelLayout const &layout)
{
	char buf[64];
	av_channel_layout_describe(&layout, buf, sizeof(buf));
	return std::string(buf);
}

std::ostream &operator<<(std::ostream &ostream, AVChannelLayout ch_layout)
{
	ostream << video::AVChannelLayoutExtension::ChannelLayoutDescription(ch_layout);
	return ostream;
}
