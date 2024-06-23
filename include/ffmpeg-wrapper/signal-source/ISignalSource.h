#pragma once
#include <ffmpeg-wrapper/AVSampleFormatExtention.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <ffmpeg-wrapper/base_include.h>
#include <ffmpeg-wrapper/info-collection/IAudioFrameInfoCollection.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <memory>

namespace video
{
	/// @brief 信号源接口。
	/// @note 一个信号源，要能够采样，所以有一个 sample 纯虚函数。此外，信号源实现了 IFrameSource 接口，
	/// 能够填充一个 AVFrame
	class ISignalSource
		: public IAudioFrameInfoCollection
	{
	public:
		virtual ~ISignalSource() = default;

		/// @brief 采样一次，返回采样结果
		/// @return
		virtual double sample() = 0;

		/// @brief 从信号源中读取帧
		/// @param frame 用来接收本信号源采样值的帧
		/// @return
		int ReadFrame(std::shared_ptr<AVFrameWrapper> frame);
	};
}
