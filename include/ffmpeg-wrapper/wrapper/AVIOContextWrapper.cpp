#include"ffmpeg-wrapper/wrapper/AVIOContextWrapper.h"
#include<ffmpeg-wrapper/ErrorCode.h>

using namespace video;

AVIOContextWrapper::AVIOContextWrapper(bool is_write, shared_ptr<base::Stream> stream)
{
	_stream = stream;
	_buffer_size = 1024 * 64;
	_buffer = (uint8_t *)av_malloc(_buffer_size);

	_wrapped_obj = avio_alloc_context(
		_buffer,
		_buffer_size,
		is_write,
		this,
		StaticReadPacket,
		StaticWritePacket,
		StaticSeek
	);
}

AVIOContextWrapper::~AVIOContextWrapper()
{
	// av_free(_buffer);
	/* 不要执行：av_free(_buffer);
	*
	* 将 _buffer 传给 avio_alloc_context 函数后，创建出来的 AVIOContext 就拥有缓冲区所有权，
	* 调用 avformat_close_input(&_wrapped_obj); 或 avformat_free_context(_wrapped_obj);
	* 时会释放这个缓冲区。所以这里如果 av_free(_buffer); 会重复释放。
	*/



	/* 因为 Stream 有自己的析构函数，且是用共享指针来引用的，所以这里不执行析构。
	* 这还有一个好处是：此 Stream 如果不止是要给本 AVIOContextWrapper 使用，
	* 还要到别的地方使用，不需要额外的逻辑来防止本 AVIOContextWrapper
	* 对象把 Stream 关闭了。
	*/
	//_stream->Close();
}

int AVIOContextWrapper::StaticReadPacket(void *opaque, uint8_t *buf, int buf_size)
{
	AVIOContextWrapper *me = (AVIOContextWrapper *)opaque;
	return me->ReadPacket(buf, buf_size);
}

int AVIOContextWrapper::StaticWritePacket(void *opaque, uint8_t const *buf, int buf_size)
{
	AVIOContextWrapper *me = (AVIOContextWrapper *)opaque;
	return me->WritePacket((uint8_t *)buf, buf_size);
}

int64_t AVIOContextWrapper::StaticSeek(void *opaque, int64_t offset, int whence)
{
	AVIOContextWrapper *me = (AVIOContextWrapper *)opaque;
	return me->Seek(offset, whence);
}

int AVIOContextWrapper::ReadPacket(uint8_t *buf, int buf_size)
{
	try
	{
		int have_read = (int)_stream->Read(buf, 0, buf_size);
		if (have_read == 0)
		{
			return (int)ErrorCode::eof;
		}

		return have_read;
	}
	catch (...)
	{
		return -1;
	}
}

int AVIOContextWrapper::WritePacket(uint8_t *buf, int buf_size)
{
	try
	{
		_stream->Write(buf, 0, buf_size);
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}

int64_t AVIOContextWrapper::Seek(int64_t offset, int whence)
{
	if (!_stream->CanSeek())
	{
		return -1;
	}

	switch (whence)
	{
	case SEEK_SET:
		{
			_stream->SetPosition(offset);
			return _stream->Position();
		}
	case SEEK_CUR:
		{
			size_t current_pos = _stream->Position();
			_stream->SetPosition(current_pos + offset);
			return _stream->Position();
		}
	case SEEK_END:
		{
			size_t end_pos = _stream->Length();
			_stream->SetPosition(end_pos + offset);
			return _stream->Position();
		}
	case AVSEEK_SIZE:
		{
			return _stream->Length();
		}
	default:
		{
			return -1;
		}
	}
}
