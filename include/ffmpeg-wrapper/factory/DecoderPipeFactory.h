#pragma once
#include<ffmpeg-wrapper/factory/IDecoderPipeFactory.h>
#include<memory>

namespace video
{
	/// <summary>
	///		本库实现的一个解码管道工厂。使用单例模式。
	/// </summary>
	class DecoderPipeFactory :
		public video::IDecoderPipeFactory
	{
	protected:
		DecoderPipeFactory() = default;

	public:
		static std::shared_ptr<DecoderPipeFactory> Instance();

		shared_ptr<IDecoderPipe> CreateDecoderPipe(AVStreamInfoCollection const &infos) override;
	};
}