#pragma once
#include <base/Wrapper.h>
#include <base/stream/Stream.h>
#include <chrono>
#include <ffmpeg-wrapper/AVCalculate.h>
#include <ffmpeg-wrapper/AVToString.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <ffmpeg-wrapper/base_include.h>
#include <ffmpeg-wrapper/info-collection/AudioFrameInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/AudioStreamInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/VideoFrameInfoCollection.h>

namespace video
{
	class ImageBuffer;

	class AVFrameWrapper
		: public base::Wrapper<AVFrame>,
		  public IAudioFrameInfoCollection,
		  public IVideoFrameInfoCollection
	{
	private:
		std::shared_ptr<ImageBuffer> _image_buf;
		AVFrame *_wrapped_obj = nullptr;

		/// @brief 为此帧分配新的缓冲区。调用这个函数必须保证一些参数已经手动设置。
		/// @note 对于视频帧，需要设置：像素格式、宽、高
		/// @note 对于音频帧，需要设置：采样格式、采样点数量、声道布局
		/// @note 本方法内部在分配新的缓冲区之前会调用 Unref 方法，避免内存泄漏。
		///
		/// @param align 内存对齐的尺寸。传入 0，会自动根据 CPU 选择对齐方式。
		/// @note 如果你不知道你在干什么，就传入 0.
		void GetBuffer(int align);

		/// @brief 让本帧引用另一个帧的缓冲区并复制其它参数。
		/// @note 在引用另一个帧之前会先调用 Unref 方法。
		///
		/// @param o
		void Ref(AVFrameWrapper const &o);

		/// @brief 解除此帧对缓冲区的引用。重复调用不会出错
		void Unref();

	public:
#pragma region 生命周期
		/// @brief 默认构造。此时构造出来的帧不含缓冲区。
		AVFrameWrapper();

		/// @brief 构造音频帧。会分配缓冲区。
		/// @param infos
		/// @param nb_samples
		AVFrameWrapper(IAudioStreamInfoCollection const &infos, int nb_samples);

		/// @brief 构造音频帧。会分配缓冲区。
		/// @param infos
		AVFrameWrapper(IAudioFrameInfoCollection const &infos);

		/// @brief 根据 infos 中的信息构造视频帧。会分配缓冲区。
		/// @param infos
		AVFrameWrapper(IVideoFrameInfoCollection const &infos);

		/// @brief 拷贝构造。
		/// @note 会复制对方的信息到自己这边。
		/// @note 会先解除自己对自己的缓冲区的引用，然后引用对方的缓冲。
		///
		/// @param o
		AVFrameWrapper(AVFrameWrapper const &o);

		~AVFrameWrapper();

		AVFrameWrapper &operator=(AVFrameWrapper const &o);
#pragma endregion

#pragma region 相等运算符
		using IAudioFrameInfoCollection::operator==;
		using IVideoFrameInfoCollection::operator==;

		/// @brief 只有本对象的指针和另一个对象的指针相等时才认为相等。
		/// @param o
		/// @return
		bool operator==(AVFrameWrapper const &o) const
		{
			return this == &o;
		}
#pragma endregion

		AVFrame *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		AVFrame *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		void ChangeTimeBase(AVRational new_time_base);

		/// @brief 获取音频数据的大小。
		/// @note 这里是根据采样格式，采样点个数，声道布局 计算出来的，且不考虑内存对齐。
		/// 所以，这里获得的是采样点实际使用的缓冲区大小，而不是分配堆内存时所分配的大小。
		///
		/// @return
		int audio_data_size();

		/// @brief 从指定位置开始将采样点设置为静音
		/// @param offset 从此位置（包括此位置）开始，对采样点设置静音
		void Mute(int offset);

		void make_writable();

		/// @brief 判断此帧是否可写
		/// @return
		bool is_writable()
		{
			return av_frame_is_writable(_wrapped_obj);
		}

		int64_t pts()
		{
			return _wrapped_obj->pts;
		}
		void set_pts(int64_t value)
		{
			_wrapped_obj->pts = value;
		}

		int64_t Duration()
		{
			return _wrapped_obj->duration;
		}
		void SetDuration(int64_t value)
		{
			_wrapped_obj->duration = value;
		}

		/// @brief 计算本帧的 pts 所对应的时间。
		/// @note 需要保证本帧的时间基被正确设置。
		///
		/// @return
		std::chrono::milliseconds PtsToMilliseconds();

		void copy_image_to_buffer(std::shared_ptr<ImageBuffer> buffer);

		/// @brief 将视频帧复制到流中.请确保本帧是视频帧。
		/// @param stream
		void CopyVideoFrameToStream(base::Stream &stream);

		/// @brief 将音频帧复制到流中。请确保本帧是音频帧。
		/// @note 调用本方法请保证帧的格式是交织的而非平面的。例如写入文件流，作为 PCM 文件，PCM
		/// 文件要求不同声道的采样点必须是交织存放的。
		///
		/// @param stream
		void CopyAudioFrameToStream(base::Stream &stream);

		/// @brief 将音频数据复制到缓冲区中。要求本帧的音频数据是交错类型的。
		/// @param buffer
		/// @param len
		void CopyAudioDataToBuffer(uint8_t *buffer, int len);

#pragma region IAudioFrameInfoCollection
		AVSampleFormat SampleFormat() const override;
		void SetSampleFormat(AVSampleFormat value) override;

		int SampleCount() const override;
		void SetSampleCount(int value) override;

		AVChannelLayout ChannelLayout() const override;
		void SetChannelLayout(AVChannelLayout value) override;

		int SampleRate() const override;
		void SetSampleRate(int value) override;

		AVRational TimeBase() const override;
		void SetTimeBase(AVRational value) override;
#pragma endregion

#pragma region IVideoFrameInfoCollection
		int Width() const override;
		void SetWidth(int value) override;

		int Height() const override;
		void SetHeight(int value) override;

		AVPixelFormat PixelFormat() const override;
		void SetPixelFormat(AVPixelFormat value) override;
#pragma endregion

		std::string ToString();
	};
}
