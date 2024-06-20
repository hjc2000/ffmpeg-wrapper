#pragma once
#include<ffmpeg-wrapper/wrapper/AVPacketWrapper.h>

namespace video
{
	class IPacketSource
	{
	public:
		virtual ~IPacketSource() {}

	public:
		/**
		 * @brief 读取包
		 * @param packet 用来接收数据的包。
		 * @return 成功返回 0，失败返回错误代码。
		*/
		virtual int ReadPacket(AVPacketWrapper &packet) = 0;
	};
}