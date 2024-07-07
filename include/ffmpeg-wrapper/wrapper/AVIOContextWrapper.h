#pragma once
#include <base/Wrapper.h>
#include <base/stream/Stream.h>
#include <ffmpeg-wrapper/base_include.h>
#include <memory>

namespace video
{
	class AVIOContextWrapper
		: public base::Wrapper<AVIOContext>
	{
	private:
		AVIOContext *_wrapped_obj = nullptr;
		int _buffer_size;
		uint8_t *_buffer;
		std::shared_ptr<base::Stream> _stream;

		static int StaticReadPacket(void *opaque, uint8_t *buf, int buf_size);
		static int StaticWritePacket(void *opaque, uint8_t const *buf, int buf_size);
		static int64_t StaticSeek(void *opaque, int64_t offset, int whence);

		int ReadData(uint8_t *buf, int buf_size);
		int WritePacket(uint8_t *buf, int buf_size);
		int64_t Seek(int64_t offset, int whence);

	public:
		AVIOContextWrapper(bool is_write, std::shared_ptr<base::Stream> stream);
		~AVIOContextWrapper();

		AVIOContext *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		AVIOContext *WrappedObj() const override
		{
			return _wrapped_obj;
		}
	};
}
