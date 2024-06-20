#pragma once
#include<base/stream/Stream.h>
#include<base/Wrapper.h>
#include<chrono>
#include<ffmpeg-wrapper/AVCalculate.h>
#include<ffmpeg-wrapper/AVToString.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/info-collection/AudioFrameInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/AudioStreamInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/VideoFrameInfoCollection.h>

namespace video
{
	class ImageBuffer;

	class AVFrameWrapper :
		public base::Wrapper<AVFrame>,
		public IAudioFrameInfoCollection,
		public IVideoFrameInfoCollection
	{
		shared_ptr<ImageBuffer> _image_buf;
		AVFrame *_wrapped_obj = nullptr;

	public:
		#pragma region 生命周期
		AVFrameWrapper();

		/// <summary>
		///		构造音频帧。会分配缓冲区。
		/// </summary>
		/// <param name="infos"></param>
		/// <param name="nb_samples"></param>
		AVFrameWrapper(IAudioStreamInfoCollection const &infos, int nb_samples);

		/// <summary>
		///		构造音频帧。会分配缓冲区。
		/// </summary>
		/// <param name="infos"></param>
		AVFrameWrapper(IAudioFrameInfoCollection const &infos);

		/// <summary>
		///		根据 infos 中的信息构造视频帧。会分配缓冲区。
		/// </summary>
		/// <param name="infos"></param>
		AVFrameWrapper(IVideoFrameInfoCollection const &infos);

		AVFrameWrapper(AVFrameWrapper const &another);

		~AVFrameWrapper();

		AVFrameWrapper &operator=(AVFrameWrapper const &another);
		#pragma endregion

		#pragma region 相等运算符
		using IAudioFrameInfoCollection::operator==;
		using IVideoFrameInfoCollection::operator==;
		/// <summary>
		///		只有本对象的指针和另一个对象的指针相等时才认为相等。
		/// </summary>
		/// <param name="another"></param>
		/// <returns></returns>
		bool operator==(AVFrameWrapper const &another) const
		{
			return this == &another;
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

		/// <summary>
		///		获取音频数据的大小。
		///		* 这里是根据采样格式，采样点个数，声道布局 计算出来的，且不考虑内存对齐。所以，
		///		  这里获得的是采样点实际使用的缓冲区大小，而不是分配堆内存时所分配的大小。
		/// </summary>
		/// <returns></returns>
		int audio_data_size();

		/// <summary>
		///		从指定位置开始将采样点设置为静音
		/// </summary>
		/// <param name="offset">从此位置（包括此位置）开始，对采样点设置静音</param>
		void mute(int offset);

		/// <summary>
		///		为此帧分配新的缓冲区。调用这个函数必须保证一些参数已经手动设置。
		///		* 对于视频帧，需要设置：像素格式、宽、高
		///		* 对于音频帧，需要设置：采样格式、采样点数量、声道布局
		/// </summary>
		/// <param name="align">
		///		内存对齐的尺寸。传入 0，会自动根据 CPU 选择对齐方式。如果你不知道你在干什么，就传入 0.
		/// </param>
		void get_buffer(int align);

		void make_writable();

		/// <summary>
		///		判断此帧是否可写
		/// </summary>
		/// <returns></returns>
		bool is_writable()
		{
			return av_frame_is_writable(_wrapped_obj);
		}

		/// <summary>
		///		让本帧引用另一个帧的缓冲区并复制其它参数。
		///		在引用另一个帧之前会先调用 unref 方法。
		/// </summary>
		/// <param name="other"></param>
		void ref(AVFrameWrapper const &other);

		/// <summary>
		///		解除此帧对缓冲区的引用。重复调用不会出错
		/// </summary>
		void unref();

		int64_t pts()
		{
			return _wrapped_obj->pts;
		}
		void set_pts(int64_t value)
		{
			_wrapped_obj->pts = value;
		}

		int64_t duration()
		{
			return _wrapped_obj->duration;
		}
		void set_duration(int64_t value)
		{
			_wrapped_obj->duration = value;
		}

		/// <summary>
		///		计算本帧的 pts 所对应的时间。
		///		* 需要保证本帧的时间基被正确设置。
		/// </summary>
		/// <returns></returns>
		std::chrono::milliseconds PtsToMilliseconds();

		void copy_image_to_buffer(shared_ptr<ImageBuffer> buffer);

		/// <summary>
		///		将视频帧复制到流中.请确保本帧是视频帧。
		/// </summary>
		/// <param name="stream"></param>
		void CopyVideoFrameToStream(base::Stream &stream);

		/// <summary>
		///		将音频帧复制到流中。请确保本帧是音频帧。
		///		注意：
		///		* 调用本方法请保证帧的格式是交织的而非平面的。例如写入文件流，作为 PCM 文件，PCM
		///		  文件要求不同声道的采样点必须是交织存放的。
		/// </summary>
		/// <param name="stream">要复制到的流</param>
		void CopyAudioFrameToStream(base::Stream &stream);

		/// <summary>
		///		将音频数据复制到缓冲区中。要求本帧的音频数据是交错类型的。
		/// </summary>
		/// <param name="buffer"></param>
		/// <param name="len"></param>
		void CopyAudioDataToBuffer(uint8_t *buffer, int len);

		std::string ToString();

		#pragma region IAudioFrameInfoCollection
		AVSampleFormat SampleFormat() const override;
		void SetSampleFormat(AVSampleFormat value) override;

		int SampleCount() const override;
		void SetSampleCount(int value) override;

		AVChannelLayout ChannelLayout() const override;
		void SetChannelLayout(AVChannelLayout value) override;

		int SampleRate() const override;
		void SetSampleRate(int value) override;

		/// <summary>
		///		AVFrameWrapper 的时间基属性不一定是有效的。例如从重采样器或解码器
		///		中输出的 AVFrameWrapper 的时间基属性就是无效值。
		/// </summary>
		/// <returns></returns>
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
	};
}
