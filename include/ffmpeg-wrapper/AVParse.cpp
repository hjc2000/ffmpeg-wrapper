#include"AVParse.h"

AVMediaType Parse(std::string str)
{
	AVMediaType type;
	if (str == "audio")
	{
		type = AVMediaType::AVMEDIA_TYPE_AUDIO;
	}
	else if (str == "video")
	{
		type = AVMediaType::AVMEDIA_TYPE_VIDEO;
	}
	else if (str == "data")
	{
		type = AVMediaType::AVMEDIA_TYPE_DATA;
	}
	else if (str == "subtitle")
	{
		type = AVMediaType::AVMEDIA_TYPE_SUBTITLE;
	}
	else if (str == "nb")
	{
		type = AVMediaType::AVMEDIA_TYPE_NB;
	}
	else
	{
		cout << CODE_POS_STR << "解析媒体类型字符串，结果为未知" << endl;
		type = AVMediaType::AVMEDIA_TYPE_UNKNOWN;
	}

	return type;
}