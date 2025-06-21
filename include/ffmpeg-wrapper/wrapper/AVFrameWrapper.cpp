#include "ffmpeg-wrapper/wrapper/AVFrameWrapper.h"
#include "AVFrameWrapper.h"
#include "ffmpeg-wrapper/ffmpeg.h"
#include <base/math/Fraction.h>
#include <base/unit/Seconds.h>
#include <ffmpeg-wrapper/AVToString.h>
#include <ffmpeg-wrapper/ErrorCode.h>
#include <ffmpeg-wrapper/ImageBuffer.h>


using namespace video;

void AVFrameWrapper::GetBuffer(int align)
{
	// GetBuffer里面不能调用 Unref 方法，否则会导致程序卡死

	/* ffmpeg 的注释里明确说了如果一个 AVFrame 已经有缓冲区了，再次调用 av_frame_get_buffer
	 * 分配缓冲区会导致内存泄漏。
	 */
	int result = av_frame_get_buffer(_wrapped_obj, align);
	if (result < 0)
	{
		std::string error_message = std::format(
			"{} av_frame_get_buffer 失败。错误代码：{}，错误消息：{}",
			CODE_POS_STR,
			result,
			base::ToString(static_cast<ErrorCode>(result)));

		throw std::runtime_error{error_message};
	}
}

void AVFrameWrapper::Ref(AVFrameWrapper const &o)
{
	Unref();
	int result = av_frame_ref(_wrapped_obj, o);
	if (result < 0)
	{
		std::string error_message = std::format(
			"{} av_frame_ref 失败。错误代码：{}，错误消息：{}",
			CODE_POS_STR,
			result,
			base::ToString(static_cast<ErrorCode>(result)));

		throw std::runtime_error{error_message};
	}
}

void AVFrameWrapper::Unref()
{
	av_frame_unref(_wrapped_obj);
}

#pragma region 构造函数

AVFrameWrapper::AVFrameWrapper()
{
	_wrapped_obj = av_frame_alloc();
}

AVFrameWrapper::AVFrameWrapper(IAudioStreamInfoCollection const &infos, int nb_samples)
	: AVFrameWrapper()
{
	IAudioStreamInfoCollection::operator=(infos);
	SetSampleCount(nb_samples);
	GetBuffer(0);
}

AVFrameWrapper::AVFrameWrapper(IAudioFrameInfoCollection const &infos)
	: AVFrameWrapper()
{
	IAudioFrameInfoCollection::operator=(infos);
	GetBuffer(0);
}

AVFrameWrapper::AVFrameWrapper(IVideoFrameInfoCollection const &infos)
	: AVFrameWrapper()
{
	IVideoFrameInfoCollection::operator=(infos);
	GetBuffer(0);
}

AVFrameWrapper::AVFrameWrapper(AVFrameWrapper const &o)
	: AVFrameWrapper()
{
	Ref(o);
}

#pragma endregion

AVFrameWrapper::~AVFrameWrapper()
{
	av_frame_free(&_wrapped_obj);
}

AVFrameWrapper &AVFrameWrapper::operator=(AVFrameWrapper const &another)
{
	Ref(another);
	return *this;
}

void AVFrameWrapper::ChangeTimeBase(AVRational new_time_base)
{
	base::Fraction fraction_old_time_base{
		_wrapped_obj->time_base.num,
		_wrapped_obj->time_base.den,
	};

	base::Fraction fraction_new_time_base{
		new_time_base.num,
		new_time_base.den,
	};

	_wrapped_obj->time_base = new_time_base;
	_wrapped_obj->pts = static_cast<int64_t>(_wrapped_obj->pts * fraction_old_time_base / fraction_new_time_base);
	_wrapped_obj->duration = static_cast<int64_t>(_wrapped_obj->duration * fraction_old_time_base / fraction_new_time_base);
}

int video::AVFrameWrapper::audio_data_size()
{
	/* 平面类型的声道布局，每一个声道的缓冲区后面都会有间隙。包类型的声道布局，多个声道的采样值
	 * 交错存放，呈 LRLRLR......
	 *
	 * 包类型的声道布局的缓冲区中，只有在最末尾有因为对齐产生的间隙；平面类型的声道布局的缓冲区中，
	 * 每一个声道的缓冲区后面都有因为对齐产生的间隙。
	 *
	 * PCM 文件需要包类型的声道布局。平面类型的声道布局仅限于 ffmpeg 内部各个函数进行数据交换
	 * 使用，不能写到文件中。
	 *
	 * 此外，要将缓冲区写入 PCM 文件时，后面的间隙不能写入文件。PCM 文件中的每一个数据都是实际的
	 * 采样值，不能是间隙，因为读的时候也是认为没有间隙的。如果把间隙写入 PCM 文件，播放的时候就会有噪音，
	 * 声音的时间也会不对。
	 *
	 * 综上，下面应该向 av_samples_get_buffer_size 函数的 align 参数传入 1，表示不对齐，也就是
	 * 获取的 buf_size 不包括后面的间隙。
	 */
	int buf_size = av_samples_get_buffer_size(nullptr,
											  ChannelLayout().nb_channels,
											  SampleCount(),
											  SampleFormat(),
											  1);
	if (buf_size < 0)
	{
		std::string error_message = std::format(
			"{} av_samples_get_buffer_size 发生错误。错误代码：{}，错误消息：{}",
			CODE_POS_STR,
			buf_size,
			base::ToString(static_cast<ErrorCode>(buf_size)));

		throw std::runtime_error{error_message};
	}

	return buf_size;
}

void video::AVFrameWrapper::Mute(int offset)
{
	av_samples_set_silence(_wrapped_obj->extended_data,
						   offset,
						   SampleCount() - offset,
						   ChannelLayout().nb_channels,
						   SampleFormat());
}

void AVFrameWrapper::MakeWritable()
{
	int result = av_frame_make_writable(_wrapped_obj);
	if (result)
	{
		std::string error_message = std::format(
			"{} av_frame_make_writable 失败。错误代码：{}，错误消息：{}",
			CODE_POS_STR,
			result,
			base::ToString(static_cast<ErrorCode>(result)));

		throw std::runtime_error{error_message};
	}
}

bool video::AVFrameWrapper::IsWritable()
{
	return av_frame_is_writable(_wrapped_obj);
}

void video::AVFrameWrapper::UpdateAudioFrameDuration()
{
	base::Fraction fraction_duration = SampleCount() * SampleInterval() / ToFraction(TimeBase());
	int64_t duration{fraction_duration.Div()};
	SetDuration(duration);
}

std::chrono::milliseconds AVFrameWrapper::PtsToMilliseconds()
{
	base::Fraction time_base{TimeBase().num, TimeBase().den};
	base::Seconds seconds{Pts() * time_base};
	return static_cast<std::chrono::milliseconds>(seconds);
}

void AVFrameWrapper::copy_image_to_buffer(std::shared_ptr<ImageBuffer> buffer)
{
	av_image_copy(buffer->_pointers,
				  buffer->_linesizes,
				  (uint8_t const **)(_wrapped_obj->data),
				  _wrapped_obj->linesize,
				  (AVPixelFormat)_wrapped_obj->format,
				  _wrapped_obj->width,
				  _wrapped_obj->height);
}

void AVFrameWrapper::CopyAudioDataToBuffer(uint8_t *buffer, int len)
{
	if (IsPlanar())
	{
		std::string error_message = std::format(
			"{} 本帧是平面类型，写入缓冲区的音频数据不允许是平面类型",
			CODE_POS_STR);

		throw std::runtime_error{error_message};
	}

	std::copy(_wrapped_obj->extended_data[0],
			  _wrapped_obj->extended_data[0] + len,
			  buffer);
}

void video::AVFrameWrapper::CopyVideoFrameToStream(base::Stream &stream)
{
	if (!_image_buf)
	{
		/* yuv 文件不能有间隙。对齐会产生间隙，一旦有间隙，间隙也会被理解成像素，就会导致花屏，
		 * 根本无法正常播放。所以这里必须使用 1 字节对齐（即不对齐）。不管写入什么流都是一样的，
		 * 统一写入不对齐的数据，包括网络流。
		 */
		_image_buf = std::shared_ptr<ImageBuffer>{
			new ImageBuffer{
				_wrapped_obj->width,
				_wrapped_obj->height,
				static_cast<AVPixelFormat>(_wrapped_obj->format),
				1,
			},
		};
	}

	copy_image_to_buffer(_image_buf);
	_image_buf->CopyToStream(stream);
}

void video::AVFrameWrapper::CopyAudioFrameToStream(base::Stream &stream)
{
	int buf_size = audio_data_size();
	stream.Write(_wrapped_obj->extended_data[0], 0, buf_size);
}

#pragma region IAudioFrameInfoCollection

AVSampleFormat video::AVFrameWrapper::SampleFormat() const
{
	return static_cast<AVSampleFormat>(_wrapped_obj->format);
}

void video::AVFrameWrapper::SetSampleFormat(AVSampleFormat value)
{
	_wrapped_obj->format = value;
}

int video::AVFrameWrapper::SampleCount() const
{
	return _wrapped_obj->nb_samples;
}

void video::AVFrameWrapper::SetSampleCount(int value)
{
	_wrapped_obj->nb_samples = value;
}

AVChannelLayout video::AVFrameWrapper::ChannelLayout() const
{
	return _wrapped_obj->ch_layout;
}

void video::AVFrameWrapper::SetChannelLayout(AVChannelLayout value)
{
	_wrapped_obj->ch_layout = value;
}

int video::AVFrameWrapper::SampleRate() const
{
	return _wrapped_obj->sample_rate;
}

void video::AVFrameWrapper::SetSampleRate(int value)
{
	_wrapped_obj->sample_rate = value;
}

AVRational video::AVFrameWrapper::TimeBase() const
{
	return _wrapped_obj->time_base;
}

void video::AVFrameWrapper::SetTimeBase(AVRational value)
{
	_wrapped_obj->time_base = value;
}

#pragma endregion

#pragma region IVideoFrameInfoCollection

int video::AVFrameWrapper::Width() const
{
	return _wrapped_obj->width;
}

void video::AVFrameWrapper::SetWidth(int value)
{
	_wrapped_obj->width = value;
}

int video::AVFrameWrapper::Height() const
{
	return _wrapped_obj->height;
}

void video::AVFrameWrapper::SetHeight(int value)
{
	_wrapped_obj->height = value;
}

AVPixelFormat video::AVFrameWrapper::PixelFormat() const
{
	return static_cast<AVPixelFormat>(_wrapped_obj->format);
}

void video::AVFrameWrapper::SetPixelFormat(AVPixelFormat value)
{
	_wrapped_obj->format = value;
}

#pragma endregion

base::Json video::AVFrameWrapper::ToJson() const
{
	base::Json root{
		{"pts", _wrapped_obj->pts},
		{
			"time_base",
			base::ToString(_wrapped_obj->time_base),
		},
	};

	if (_wrapped_obj->width == 0)
	{
		// 通过 width==0 来推测是音频帧
		root["sample_format"] = base::ToString(SampleFormat());
	}

	return root;
}
