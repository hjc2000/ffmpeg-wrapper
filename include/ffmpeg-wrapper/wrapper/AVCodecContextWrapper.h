#pragma once
#include<ffmpeg-wrapper/AVCodecExtention.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/IAudioFrameInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameSource.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include<base/Wrapper.h>

namespace video
{
	class AVFrameWrapper;
	class AVPacketWrapper;

	/// <summary>
	///		* 本类通过工厂函数创建，构造函数都是私有的。
	///		* 本类线程安全，所有公共方法都有加锁。
	/// </summary>
	class AVCodecContextWrapper :
		public base::Wrapper<AVCodecContext>,
		public IAudioStreamInfoCollection,
		public IVideoStreamInfoCollection,
		public IFrameSource,
		public IPacketConsumer
	{
		#pragma region AVCodec
		/// <summary>
		///		与本类绑定的 AVCodec
		/// </summary>
		AVCodec *_codec = nullptr;

		AVCodec const *Codec() const
		{
			return _codec;
		}

		void SetCodec(AVCodec const *codec)
		{
			_codec = (AVCodec *)codec;
		}
		#pragma endregion

		AVCodecContext *_wrapped_obj = nullptr;

		AVCodecContextWrapper(AVCodec const *codec);
		AVCodecContextWrapper(AVCodec const *codec, AVCodecParameters *param);

	public:
		~AVCodecContextWrapper();

		AVCodecContext *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		AVCodecContext *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		#pragma region 工厂函数
		/// <summary>
		///		创建解码器。此函数创建的解码器是通用的，音频流和视频流的解码器都是用这个函数创建。
		///		* 会将流的 AVCodecParameters 复制到解码器上下文中
		///		* 会将流的时间基复制到解码器上下文中。
		///		* 会将流的帧率复制到解码器上下文中。（虽然帧率只对视频流有效，但音频流反正不会用到，复制了也没事）
		///		* 返回的解码器将处于打开状态
		/// </summary>
		/// <param name="stream"></param>
		/// <returns></returns>
		static shared_ptr<AVCodecContextWrapper> CreateDecoder(AVStreamInfoCollection stream);

		/// <summary>
		///		创建音频编码器
		/// </summary>
		/// <param name="encoder_name">通过编码器的名称查找编码器</param>
		/// <param name="infos">
		///		音频流信息。将会利用此流的信息创建编码器。
		///		* 所利用的信息包括时间基。
		/// </param>
		/// <param name="set_global_header">
		///		是否需要设置全局头部。
		///		* 某些封装格式要求编码器设置全局头部。可以通过检查 AVFormatContextWrapper 的
		///		  NeedGlobalHeader 属性来确定是否需要全局头部。
		/// </param>
		/// <returns></returns>
		static shared_ptr<AVCodecContextWrapper> CreateEncoder(
			const char *encoder_name,
			IAudioStreamInfoCollection const &infos,
			bool set_global_header,
			bool auto_open = true
		);

		/**
		* @brief 创建视频编码器。返回的编码器已打开。
		* @param encoder_name 通过编码器的名称查找编码器
		* @param infos 视频流信息。将会利用此流的信息创建编码器。所利用的信息包括时间基。
		*
		* @param set_global_header 是否需要设置全局头部。
		* * 某些封装格式要求编码器设置全局头部。可以通过检查 AVFormatContextWrapper 的
		* NeedGlobalHeader 属性来确定是否需要全局头部。
		*
		* @return 返回的编码器已打开。
		*/
		static shared_ptr<AVCodecContextWrapper> CreateEncoder(
			const char *encoder_name,
			IVideoStreamInfoCollection const &infos,
			bool set_global_header,
			bool auto_open = true
		);
		#pragma endregion

		/// <summary>
		///		打开编解码器。
		///		- 打开前一定要先设置编解码器的参数。
		/// </summary>
		/// <param name="dic"></param>
		void Open(AVDictionary **dic = nullptr);

		void SetCodecParams(AVCodecParameters *param);

		/// <summary>
		///		设置全局头部
		///		- 某些封装格式要求编码器在编码的时候设置全局头部。要知道封装格式是不是需要设置全局头部，
		///		  可以查看 AVFormatContextWrapper 的 NeedGlobalHeader 属性。
		/// </summary>
		void SetGlobalHeader()
		{
			_wrapped_obj->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		}

		#pragma region 编码
		void SendFrame(AVFrameWrapper *frame);
		int ReadPacket(AVPacketWrapper &packet);
		#pragma endregion

		#pragma region 解码
		void SendPacket(AVPacketWrapper *packet) override;
		int ReadFrame(AVFrameWrapper &frame) override;
		#pragma endregion

		#pragma region IAudioStreamInfoCollection, IVideoStreamInfoCollection
		AVChannelLayout ChannelLayout() const override;
		void SetChannelLayout(AVChannelLayout value) override;
		AVSampleFormat SampleFormat() const override;
		void SetSampleFormat(AVSampleFormat value) override;
		int SampleRate() const override;
		void SetSampleRate(int value) override;

		int Width() const override;
		void SetWidth(int value) override;
		int Height() const override;
		void SetHeight(int value) override;
		AVPixelFormat PixelFormat() const override;
		void SetPixelFormat(AVPixelFormat value) override;
		/// <summary>
		///		获取此码器的时间基。作为编码器，时间基必须由用户手动设置，作为解码器，时间基是
		///		无用的，无效的，不要试图从解码器中获取此参数。
		/// </summary>
		/// <returns></returns>
		AVRational TimeBase() const override;
		void SetTimeBase(AVRational value) override;
		AVRational FrameRate() const override;
		void SetFrameRate(AVRational value) override;
		#pragma endregion
	};
}
