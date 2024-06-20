#pragma once
#include<ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>

namespace video
{
	/// <summary>
	///		AVStream 会随着 AVFormatContex 的销毁而销毁，所以不能长久保留 AVStream 的指针，
	///		这会导致访问失效内存。所以设计了本类，用来储存 AVStream 中的信息。这是基于值的复制，
	///		深度拷贝。
	/// </summary>
	class AVStreamInfoCollection :
		public IAudioStreamInfoCollection,
		public IVideoStreamInfoCollection
	{
	private:
		void CopyCodecParamFrom(AVCodecParameters const *src);

	public:
		AVStreamInfoCollection() = default;
		AVStreamInfoCollection(AVStreamInfoCollection const &stream);
		AVStreamInfoCollection(AVStreamWrapper const &stream);
		~AVStreamInfoCollection();
		AVStreamInfoCollection &operator=(AVStreamInfoCollection const &stream);
		AVStreamInfoCollection &operator=(AVStreamWrapper const &stream);

		#pragma region 相等运算符
		using IAudioStreamInfoCollection::operator==;
		using IVideoStreamInfoCollection::operator==;
		/// <summary>
		///		当且仅当两个对象的指针相等时才相等。不能认为本类对象作为
		///		IAudioStreamInfoCollection 和 IVideoStreamInfoCollection
		///		都判断相等时就相等了。因为本类中还有很多其他字段。而且判断这么多
		///		字段会引起性能问题。
		/// </summary>
		/// <param name="another"></param>
		/// <returns></returns>
		bool operator==(AVStreamInfoCollection const &another) const
		{
			return this == &another;
		}
		#pragma endregion

		int _index = -1;
		int Index()  const;
		void SetIndex(int value);

		AVMediaType _media_type { };

		int64_t _bitrate = 0;
		int64_t Bitrate() const
		{
			return _bitrate;
		}
		void SetBitrate(int64_t value)
		{
			_bitrate = value;
		}

		AVCodecParameters *_codec_params = avcodec_parameters_alloc();
		AVCodec const *_codec = nullptr;

		AVRational _timebase { };
		AVSampleFormat _sample_format { };
		int _sample_rate = 0;
		AVChannelLayout _ch_layout { };
		int _width = 0;
		int _height = 0;
		AVPixelFormat _pixel_format { };
		AVRational _frame_rate { };

		#pragma region 通过 IAudioStreamInfoCollection 继承
		AVRational TimeBase() const override;
		void SetTimeBase(AVRational value) override;
		AVSampleFormat SampleFormat() const override;
		void SetSampleFormat(AVSampleFormat value) override;
		int SampleRate() const override;
		void SetSampleRate(int value) override;
		AVChannelLayout ChannelLayout() const override;
		void SetChannelLayout(AVChannelLayout value) override;
		#pragma endregion

		#pragma region 通过 IVideoStreamInfoCollection 继承
		int Width() const override;
		void SetWidth(int value) override;
		int Height() const override;
		void SetHeight(int value) override;
		AVPixelFormat PixelFormat() const override;
		void SetPixelFormat(AVPixelFormat value) override;
		AVRational FrameRate() const override;
		void SetFrameRate(AVRational value) override;
		#pragma endregion
	};
}
