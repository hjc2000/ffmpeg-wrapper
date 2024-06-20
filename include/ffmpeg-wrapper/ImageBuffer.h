#pragma once
#include<base/stream/Stream.h>
#include<ffmpeg-wrapper/base_include.h>
#include<fstream>
#include<memory>
using std::shared_ptr;

namespace video
{
	class AVFrameWrapper;

	/// <summary>
	/// 配合 AVFrameWrapper 使用。封装字节数组，用来储存一帧图像
	/// </summary>
	class ImageBuffer
	{
	public:
		ImageBuffer(int width, int height, AVPixelFormat format, int align);
		~ImageBuffer();

		void CopyToStream(base::Stream &stream);

		/// <summary>
		/// 长度为 4 的指针数组。每个指针都指向一个数组的首地址。4 个数组可能分别代表
		/// YUV 三个通道一个透明度通道。也有可能是 RGBA
		/// </summary>
		uint8_t *_pointers[4];

		/// <summary>
		/// 用来指示 _pointers 指向的各个数组的长度
		/// </summary>
		int _linesizes[4];

		/// <summary>
		/// _pointers 指向的所有数组的长度之和。_pointers 指向的所有数组在堆内存中
		/// 是连续的，即第一个数组的最后一个字节紧跟着第二个数组的第一个字节，以此类推。
		/// 将整个缓冲区写入文件中时也只需要取出 _pointers[0] 的指针，这个就是整个缓冲区
		/// 的首地址，然后将缓冲区当成一个长度为 _size 的一维数组写入到文件中。
		/// </summary>
		int _size;
	};
}
