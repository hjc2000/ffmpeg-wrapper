#pragma once
#include <ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include <ffmpeg-wrapper/pipe/interface/IDecoderPipe.h>
#include <memory>

namespace video
{
	/// <summary>
	///		本库实现的一个解码管道工厂。使用单例模式。
	/// </summary>
	class DecoderPipeFactory
	{
	protected:
		DecoderPipeFactory() = default;

	public:
		static std::shared_ptr<DecoderPipeFactory> Instance();

		std::shared_ptr<IDecoderPipe> CreateDecoderPipe(
			AVStreamInfoCollection const &infos);
	};
}
