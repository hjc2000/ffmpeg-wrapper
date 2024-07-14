#pragma once
#include <ffmpeg-wrapper/base_include.h>

namespace video
{
	class AVCodecExtension
	{
	private:
		AVCodecExtension() = delete;

	public:
		static AVCodec const *FindEncoderById(AVCodecID id);

		/**
		 * @brief 通过编码器的名字查找编码器
		 * @param name
		 * @return 成功返回指向编码器的指针。失败返回空指针。
		 */
		static AVCodec const *FindEncoderByName(char const *name);

		/**
		 * @brief 通过解码器的 ID 查找解码器
		 * @param id
		 * @return 成功则返回指向解码器的指针。失败则返回空指针。
		 */
		static AVCodec const *FindDecoderById(AVCodecID id);

		static AVCodec const *FindDecoderByName(char const *name);
	};
}