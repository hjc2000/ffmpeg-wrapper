#pragma once
#include <base/Wrapper.h>
#include <ffmpeg-wrapper/AVCalculate.h>
#include <ffmpeg-wrapper/base_include.h>

namespace video
{
	class AVFrameWrapper;
	class AVCodecContextWrapper;

	class AVPacketWrapper
		: public base::Wrapper<AVPacket>
	{
	private:
		AVPacket *_wrapped_obj = nullptr;

		/// <summary>
		///		让本包引用另一个包的缓冲区，并且复制其它参数。
		///		在引用另一个包之前会先调用 unref 方法。
		/// </summary>
		/// <param name="other"></param>
		void Ref(const AVPacketWrapper &other);

		/// <summary>
		///		解除此包对缓冲区的引用
		/// </summary>
		void Unref();

	public:
		AVPacketWrapper();
		AVPacketWrapper(AVPacketWrapper const &another);
		~AVPacketWrapper();
		AVPacketWrapper &operator=(AVPacketWrapper const &another);

		AVPacket *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		AVPacket *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		void ChangeTimeBase(AVRational new_time_base);

		int StreamIndex() const;
		void SetStreamIndex(int value);

		int64_t Duration() const;
		void SetDuration(int64_t value);

		int64_t Pts() const;
		void SetPts(int64_t value);

		int64_t Dts() const;
		void SetDts(int64_t value);

		AVRational TimeBase() const;
		void SetTimeBase(AVRational value);
	};
}