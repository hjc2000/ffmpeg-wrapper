#pragma once
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<sdl2-wrapper/SDL_Initializer.h>

namespace video
{
	class AVPixelFormatExtension
	{
		AVPixelFormatExtension() = delete;

	public:
		static SDL_PixelFormatEnum AVPixelFormat_to_SDL_PixelFormatEnum(AVPixelFormat fmt)
		{
			switch (fmt)
			{
			case AVPixelFormat::AV_PIX_FMT_YUV420P:
				{
					return SDL_PixelFormatEnum::SDL_PIXELFORMAT_IYUV;
				}
			case AVPixelFormat::AV_PIX_FMT_YUV422P:
				{
					return SDL_PixelFormatEnum::SDL_PIXELFORMAT_YUY2;
				}
			case  AVPixelFormat::AV_PIX_FMT_RGB24:
				{
					return SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGB888;
				}
			case AVPixelFormat::AV_PIX_FMT_YUV420P10LE:
				{
					throw std::runtime_error{ CODE_POS_STR + std::string("暂不支持") };
				}
			default:
				{
					throw std::runtime_error{ CODE_POS_STR + std::string("暂不支持") };
				}
			}
		}

		static AVPixelFormat SDL_PixelFormatEnum_to_AVPixelFormat(SDL_PixelFormatEnum fmt)
		{
			switch (fmt)
			{
			case SDL_PixelFormatEnum::SDL_PIXELFORMAT_IYUV:
				{
					return AVPixelFormat::AV_PIX_FMT_YUV420P;
				}
			case SDL_PixelFormatEnum::SDL_PIXELFORMAT_YUY2:
				{
					return AVPixelFormat::AV_PIX_FMT_YUV422P;
				}
			case SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGB888:
				{
					return AVPixelFormat::AV_PIX_FMT_RGB24;
				}
			default:
				{
					throw std::runtime_error{ CODE_POS_STR + std::string("暂不支持") };
				}
			}
		}
	};
}