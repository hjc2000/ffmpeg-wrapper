#pragma once
#include "ffmpeg-wrapper/ffmpeg.h"
#include <base/stream/Stream.h>


namespace video
{
	class AVFrameWrapper;

	/// @brief 配合 AVFrameWrapper 使用。封装字节数组，用来储存一帧图像
	class ImageBuffer
	{
	public:
		ImageBuffer(int width, int height, AVPixelFormat format, int align);
		~ImageBuffer();

		void CopyToStream(base::Stream &stream);

		/// @brief 长度为 4 的指针数组。每个指针都指向一个数组的首地址。4 个数组可能分别代表
		/// YUV 三个通道一个透明度通道。也有可能是 RGBA
		uint8_t *_pointers[4];

		/// @brief 用来指示 _pointers 指向的各个数组的长度
		int _linesizes[4];

		/// @brief _pointers 指向的所有数组的长度之和。_pointers 指向的所有数组在堆内存中
		/// 是连续的，即第一个数组的最后一个字节紧跟着第二个数组的第一个字节，以此类推。
		/// 将整个缓冲区写入文件中时也只需要取出 _pointers[0] 的指针，这个就是整个缓冲区
		/// 的首地址，然后将缓冲区当成一个长度为 _size 的一维数组写入到文件中。
		int _size;
	};
} // namespace video
