#pragma once
#include<ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include<ffmpeg-wrapper/AVSampleFormatExtention.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/info-collection/IAudioFrameInfoCollection.h>

namespace video
{
	/// <summary>
	///		信号源接口。
	///		* 一个信号源，要能够采样，所以有一个 sample 纯虚函数。此外，信号源实现了 IFrameSource 接口，
	///		  能够填充一个 AVFrame
	/// </summary>
	class ISignalSource : public IAudioFrameInfoCollection
	{
	public:
		virtual ~ISignalSource() {}

	public:
		/**
		 * @brief 采样一次，返回采样结果
		*/
		virtual double sample() = 0;

		/// <summary>
		///		从信号源中读取帧
		/// </summary>
		/// <param name="frame">用来接收本信号源采样值的帧</param>
		/// <returns>目前总是会返回 0</returns>
		int ReadFrame(shared_ptr<AVFrameWrapper> frame);
	};
}