#pragma once
#include <base/Wrapper.h>
#include <base/pipe/IConsumer.h>
#include <base/pipe/ISource.h>
#include <ffmpeg-wrapper/base_include.h>
#include <ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/IAudioFrameInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include <memory>

namespace video
{
	class AVFrameWrapper;
	class AVPacketWrapper;

	/// @brief 编解码器上下文的封装
	/// @note 本类通过工厂函数创建，构造函数都是私有的。
	class AVCodecContextWrapper
		: public base::Wrapper<AVCodecContext>,
		  public IAudioStreamInfoCollection,
		  public IVideoStreamInfoCollection
	{
	private:
		AVCodec const *_codec = nullptr;
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
		/// @brief 创建解码器。此函数创建的解码器是通用的，音频流和视频流的解码器都是用这个函数创建。
		/// @note 会将流的 AVCodecParameters 复制到解码器上下文中
		/// @note 会将流的时间基复制到解码器上下文中。
		/// @note 会将流的帧率复制到解码器上下文中。（虽然帧率只对视频流有效，但音频流反正不会用到，复制了也没事）
		/// @note 返回的解码器将处于打开状态
		///
		/// @param stream
		/// @return
		static std::shared_ptr<AVCodecContextWrapper> CreateDecoder(AVStreamInfoCollection stream);

		/// @brief 创建音频编码器
		/// @param encoder_name 通过编码器的名称查找编码器
		///
		/// @param infos 音频流信息。将会利用此流的信息创建编码器。
		/// @note 所利用的信息包括时间基。
		///
		/// @param set_global_header 是否需要设置全局头部。
		/// @note 某些封装格式要求编码器设置全局头部。可以通过检查 AVFormatContextWrapper 的
		/// NeedGlobalHeader 属性来确定是否需要全局头部。
		///
		/// @param auto_open 是否自动打开编码器
		///
		/// @return
		static std::shared_ptr<AVCodecContextWrapper> CreateEncoder(
			char const *encoder_name,
			IAudioStreamInfoCollection const &infos,
			bool set_global_header,
			bool auto_open = true);

		/// @brief 创建视频编码器。
		/// @param encoder_name 通过编码器的名称查找编码器
		///
		/// @param infos 视频流信息。
		/// @note 将会利用此流的信息创建编码器。所利用的信息包括时间基。
		///
		/// @param set_global_header 是否需要设置全局头部。
		/// @note 某些封装格式要求编码器设置全局头部。可以通过检查 AVFormatContextWrapper 的
		/// NeedGlobalHeader 属性来确定是否需要全局头部。
		///
		/// @param auto_open 是否自动打开编码器
		///
		/// @return
		static std::shared_ptr<AVCodecContextWrapper> CreateEncoder(
			char const *encoder_name,
			IVideoStreamInfoCollection const &infos,
			bool set_global_header,
			bool auto_open = true);
#pragma endregion

		/// @brief 打开编解码器。
		/// @note 打开前一定要先设置编解码器的参数。
		///
		/// @param dic
		void Open(AVDictionary **dic = nullptr);

		/// @brief 通过 AVCodecParameters 设置编解码器参数。
		/// @note AVCodecParameters 里面没有时间基信息。
		///
		/// @param param
		void SetCodecParams(AVCodecParameters *param);

		/// @brief 设置全局头部
		/// @note 某些封装格式要求编码器在编码的时候设置全局头部。要知道封装格式是不是需要设置全局头部，
		/// 可以查看 AVFormatContextWrapper 的 NeedGlobalHeader 属性。
		void SetGlobalHeader()
		{
			_wrapped_obj->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		}

#pragma region 编码
		void SendFrame(AVFrameWrapper *frame);
		int ReadPacket(AVPacketWrapper &packet);
#pragma endregion

#pragma region 解码
		void SendPacket(AVPacketWrapper *packet);
		int ReadFrame(AVFrameWrapper &frame);
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

		/// @brief 获取此码器的时间基。
		/// @return
		AVRational TimeBase() const override;
		void SetTimeBase(AVRational value) override;

		AVRational FrameRate() const override;
		void SetFrameRate(AVRational value) override;
#pragma endregion
	};

	AVCodec const *FindEncoderById(AVCodecID id);

	/**
	 * @brief 通过编码器的名字查找编码器
	 * @param name
	 * @return 成功返回指向编码器的指针。失败返回空指针。
	 */
	AVCodec const *FindEncoderByName(char const *name);

	/**
	 * @brief 通过解码器的 ID 查找解码器
	 * @param id
	 * @return 成功则返回指向解码器的指针。失败则返回空指针。
	 */
	AVCodec const *FindDecoderById(AVCodecID id);

	AVCodec const *FindDecoderByName(char const *name);
}
