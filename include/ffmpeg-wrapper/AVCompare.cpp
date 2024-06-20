#include<ffmpeg-wrapper/AVCompare.h>

bool operator==(AVRational const a, AVRational const b)
{
	return (a.num == b.num) && (a.den == b.den);
}

bool operator==(AVChannelLayout const ch1, AVChannelLayout const ch2)
{
	return ch1.nb_channels == ch2.nb_channels &&
		ch1.order == ch2.order;
}
