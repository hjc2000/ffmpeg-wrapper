#include "ffmpeg-wrapper/ImageBuffer.h"
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/wrapper/AVFrameWrapper.h>

using namespace video;

video::ImageBuffer::ImageBuffer(int width, int height, AVPixelFormat format, int align)
{
	// align 的意思是对齐的字节数
	_size = av_image_alloc(_pointers, _linesizes,
		width, height,
		format, align);

	if (_size < 0)
	{
		throw std::runtime_error{ ToString((ErrorCode)_size) };
	}
}

video::ImageBuffer::~ImageBuffer()
{
	cout << "ImageBuffer 析构函数" << endl;
	::av_freep(&_pointers[0]);
}

void video::ImageBuffer::CopyToStream(base::Stream &stream)
{
	/*
	* _pointers 是一个长度为 4 的指针数组，里面储存着 4 个指针。
	* _pointers 也可以当作是一个二维数组。只不过每一行的长度可能不同。
	*
	* _pointers[0] 是这个指针数组里的第一个指针，指向这个二维数组的第一行的头部。
	* 因为 _pointers 这个二维数组的所有单元格在内存上是连续的（一行接着一行，
	* 第二行的位置紧跟在第一行后面），所以 _pointers[0] 内的指针指向的是整个缓冲区的头部，
	* 而 _size 是整个缓冲区的大小（每一行的长度相加）。
	* 下面的这句就相当于在循环中遍历 _pointers 中的每一行并写入文件。
	*/
	stream.Write(_pointers[0], 0, _size);
}
