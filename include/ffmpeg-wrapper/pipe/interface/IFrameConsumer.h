#pragma once
#include <base/pipe/IConsumer.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>

namespace video
{
	class IFrameConsumer
	{
	public:
		virtual ~IFrameConsumer() = default;

		/// <summary>
		///		送入帧
		/// </summary>
		/// <param name="frame">
		///		要送入的帧。送入空指针表示冲洗。
		///		冲洗完后就不要再送入帧了，否则会引发异常。
		/// </param>
		virtual void SendData(AVFrameWrapper *frame) = 0;
	};
}