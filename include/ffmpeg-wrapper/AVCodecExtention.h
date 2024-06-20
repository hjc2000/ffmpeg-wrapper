#pragma once
#include<base/Wrapper.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/base_include.h>

namespace video
{
	class AVCodecExtention
	{
	public:
		static const AVCodec *find_encoder_by_id(AVCodecID id);

		/**
		 * @brief 通过编码器的名字查找编码器
		 * @param name
		 * @return 成功返回指向编码器的指针。失败返回空指针。
		*/
		static const AVCodec *find_encoder_by_name(const char *name);

		/**
		 * @brief 通过解码器的 ID 查找解码器
		 * @param id
		 * @return 成功则返回指向解码器的指针。失败则返回空指针。
		*/
		static const AVCodec *find_decoder_by_id(AVCodecID id);

		static const AVCodec *find_decoder_by_name(const char *name);
	};
}
