#include "ffmpeg-wrapper/wrapper/AVIOContextWrapper.h"
#include <ffmpeg-wrapper/ErrorCode.h>

using namespace video;

AVIOContextWrapper::AVIOContextWrapper(AVIOContextWrapper_IsWrite const &is_write,
                                       std::shared_ptr<base::Stream> stream)
{
    _stream = stream;
    _buffer_size = 1024 * 64;
    _buffer = static_cast<uint8_t *>(av_malloc(_buffer_size));

    _wrapped_obj = avio_alloc_context(_buffer,
                                      _buffer_size,
                                      is_write.Value(),
                                      this,
                                      StaticReadPacket,
                                      StaticWritePacket,
                                      StaticSeek);
}

AVIOContextWrapper::~AVIOContextWrapper()
{
    /* 不要执行：av_free(_buffer);
     *
     * 将 _buffer 传给 avio_alloc_context 函数后，创建出来的 AVIOContext 就拥有缓冲区所有权，
     * 调用 avformat_close_input(&_wrapped_obj); 或 avformat_free_context(_wrapped_obj);
     * 时会释放这个缓冲区。所以这里如果 av_free(_buffer); 会重复释放。
     */
}

int AVIOContextWrapper::StaticReadPacket(void *opaque, uint8_t *buf, int buf_size)
{
    AVIOContextWrapper *self = static_cast<AVIOContextWrapper *>(opaque);
    return self->ReadData(buf, buf_size);
}

int AVIOContextWrapper::StaticWritePacket(void *opaque, uint8_t const *buf, int buf_size)
{
    AVIOContextWrapper *self = static_cast<AVIOContextWrapper *>(opaque);
    return self->WritePacket((uint8_t *)buf, buf_size);
}

int64_t AVIOContextWrapper::StaticSeek(void *opaque, int64_t offset, int whence)
{
    AVIOContextWrapper *self = static_cast<AVIOContextWrapper *>(opaque);
    return self->Seek(offset, whence);
}

int AVIOContextWrapper::ReadData(uint8_t *buf, int buf_size)
{
    try
    {
        int have_read = _stream->Read(buf, 0, buf_size);
        if (have_read == 0)
        {
            return static_cast<int>(ErrorCode::eof);
        }

        return have_read;
    }
    catch (std::exception const &e)
    {
        std::cout << CODE_POS_STR << e.what() << std::endl;
        return -1;
    }
    catch (...)
    {
        std::cout << CODE_POS_STR << "发生了未知异常" << std::endl;
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
    catch (std::exception const &e)
    {
        std::cout << CODE_POS_STR << e.what() << std::endl;
        return -1;
    }
    catch (...)
    {
        std::cout << CODE_POS_STR << "发生了未知异常" << std::endl;
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
