#pragma once
#include<ffmpeg-wrapper/wrapper/AVFrameWrapper.h>

namespace video
{
	class IFrameSource
	{
	public:
		virtual ~IFrameSource() {}

	public:
		/**
		 * @brief 读取帧
		 * @param frame 用来接收数据的帧
		 * @return 成功返回 0，失败返回负数的错误代码
		*/
		virtual int ReadFrame(AVFrameWrapper &frame) = 0;
	};
}