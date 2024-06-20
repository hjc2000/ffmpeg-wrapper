#pragma once
#include<ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IDecoderPipe.h>

namespace video
{
	/// <summary>
	///		解码器管道工厂接口。
	///		* 不同平台有不同的硬件加速解码器，用户可以自己实现解码器管道工厂，作为依赖注入到本库的一些类中。
	/// </summary>
	class IDecoderPipeFactory
	{
	public:
		virtual ~IDecoderPipeFactory() = default;

		virtual shared_ptr<IDecoderPipe> CreateDecoderPipe(AVStreamInfoCollection const &infos) = 0;
	};
}