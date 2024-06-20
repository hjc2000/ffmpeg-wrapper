#pragma once
#include<ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPipeFrameSource.h>
#include<jccpp/IDisposable.h>

namespace video
{
	class IDecoderPipe :
		public IDisposable,
		public IPacketConsumer,
		public IPipeFrameSource,
		public IAudioStreamInfoCollection,
		public IVideoStreamInfoCollection
	{
	public:
		virtual ~IDecoderPipe() = default;

		/// <summary>
		///		冲洗解码器，但是不冲洗消费者。
		///		当需要把本解码器撤掉，换另一个解码器时就要调用本方法。这样既可以取出残留在解码器
		///		中的帧，又不会把下级的消费者也一起冲洗了。
		/// </summary>
		virtual void FlushDecoderButNotFlushConsumers() = 0;

		/// <summary>
		///		因为 IAudioStreamInfoCollection 和 IVideoStreamInfoCollection
		///		都有 TimeBase 和 SetTimeBase，所以这里重复一下，否则会报错，说不明确。
		///		这里重复后，就可以同时重写 IAudioStreamInfoCollection 和 IVideoStreamInfoCollection
		///		中的 TimeBase 和 SetTimeBase。
		/// </summary>
		/// <returns></returns>
		virtual AVRational TimeBase() const = 0;
		virtual void SetTimeBase(AVRational value) = 0;
	};
}