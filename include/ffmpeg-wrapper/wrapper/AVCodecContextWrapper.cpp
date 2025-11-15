#include "AVCodecContextWrapper.h"
#include "ffmpeg-wrapper/ErrorCode.h"
#include "ffmpeg-wrapper/ffmpeg.h"
#include "ffmpeg-wrapper/wrapper/AVFrameWrapper.h"
#include "ffmpeg-wrapper/wrapper/AVPacketWrapper.h"
#include "ffmpeg-wrapper/wrapper/AVStreamWrapper.h"
#include <format>
#include <iostream>
#include <memory>

#pragma region 构造函数

video::AVCodecContextWrapper::AVCodecContextWrapper(AVCodec const *codec)
{
	_codec = codec;
	_wrapped_obj = avcodec_alloc_context3(codec);
	if (!_wrapped_obj)
	{
		throw std::runtime_error("AVCodecContextWrapper create(AVCodecWrapper codec) 失败");
	}
}

video::AVCodecContextWrapper::AVCodecContextWrapper(AVCodec const *codec, AVCodecParameters *param)
	: AVCodecContextWrapper(codec)
{
	SetCodecParams(param);
}

#pragma endregion

video::AVCodecContextWrapper::~AVCodecContextWrapper()
{
	avcodec_free_context(&_wrapped_obj);
}

#pragma region 工厂函数

std::shared_ptr<video::AVCodecContextWrapper> video::AVCodecContextWrapper::CreateDecoder(AVStreamInfoCollection stream)
{
	std::shared_ptr<AVCodecContextWrapper> context_wrapper{
		new AVCodecContextWrapper{
			stream._codec,
			stream._codec_params,
		},
	};

	context_wrapper->SetTimeBase(stream.TimeBase());
	context_wrapper->SetFrameRate(stream.FrameRate());

	context_wrapper->Open();
	return context_wrapper;
}

std::shared_ptr<video::AVCodecContextWrapper> video::AVCodecContextWrapper::CreateEncoder(
	char const *encoder_name,
	IAudioStreamInfoCollection const &infos,
	bool set_global_header,
	bool auto_open)
{
	AVCodec const *codec = avcodec_find_encoder_by_name(encoder_name);
	if (codec == nullptr)
	{
		throw std::runtime_error{CODE_POS_STR + std::string{"查找编码器失败"}};
	}

	std::shared_ptr<AVCodecContextWrapper> context_wrapper{
		new AVCodecContextWrapper{
			codec,
		},
	};

	// 设置编码器参数
	(*context_wrapper)->codec_type = AVMediaType::AVMEDIA_TYPE_AUDIO;
	(*context_wrapper)->ch_layout = infos.ChannelLayout();
	(*context_wrapper)->sample_fmt = infos.SampleFormat();
	(*context_wrapper)->sample_rate = infos.SampleRate();
	(*context_wrapper)->time_base = infos.TimeBase();

	if (set_global_header)
	{
		context_wrapper->SetGlobalHeader();
	}

	if (auto_open)
	{
		context_wrapper->Open();
	}

	return context_wrapper;
}

std::shared_ptr<video::AVCodecContextWrapper> video::AVCodecContextWrapper::CreateEncoder(
	char const *encoder_name,
	IVideoStreamInfoCollection const &infos,
	bool set_global_header,
	bool auto_open)
{
	AVCodec const *codec = avcodec_find_encoder_by_name(encoder_name);
	if (codec == nullptr)
	{
		throw std::runtime_error{CODE_POS_STR + std::string{"查找编码器失败"}};
	}

	std::shared_ptr<AVCodecContextWrapper> context_wrapper{new AVCodecContextWrapper{codec}};

	// 设置编码器参数
	(*context_wrapper)->codec_type = AVMediaType::AVMEDIA_TYPE_VIDEO;

	(*context_wrapper)->width = infos.Width();
	(*context_wrapper)->height = infos.Height();
	(*context_wrapper)->pix_fmt = infos.PixelFormat();

	(*context_wrapper)->time_base = infos.TimeBase();
	(*context_wrapper)->framerate = infos.FrameRate();

	(*context_wrapper)->gop_size = infos.FrameRate().num / infos.FrameRate().den;
	//(*ctx)->max_b_frames = 10;
	if (set_global_header)
	{
		context_wrapper->SetGlobalHeader();
	}

	if (auto_open)
	{
		context_wrapper->Open();
	}

	return context_wrapper;
}

#pragma endregion

void video::AVCodecContextWrapper::Open(AVDictionary **dic)
{
	int ret = ::avcodec_open2(_wrapped_obj, _codec, dic);
	if (ret)
	{
		throw std::runtime_error{CODE_POS_STR + std::string{"打开编解码器失败"}};
	}
}

void video::AVCodecContextWrapper::SetCodecParams(AVCodecParameters *param)
{
	int ret = ::avcodec_parameters_to_context(_wrapped_obj, param);
	if (ret < 0)
	{
		throw std::runtime_error("AVCodecContextWrapper::set_encoder_param");
	}
}

void video::AVCodecContextWrapper::SetGlobalHeader()
{
	_wrapped_obj->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

#pragma region 编码

void video::AVCodecContextWrapper::SendFrame(AVFrameWrapper *frame)
{
	int ret;
	if (frame)
	{
		// 正常送入帧
		if (frame->TimeBase() != TimeBase())
		{
			frame->ChangeTimeBase(TimeBase());
		}

		ret = ::avcodec_send_frame(_wrapped_obj, *frame);
	}
	else
	{
		// 送入空指针，表示冲洗编码器
		ret = ::avcodec_send_frame(_wrapped_obj, nullptr);
	}

	if (ret)
	{
		std::string msg = std::format("送入帧失败，错误代码：{} —— {}",
									  ret,
									  base::ToString((ErrorCode)ret));

		throw std::runtime_error{CODE_POS_STR + msg};
	}
}

int video::AVCodecContextWrapper::ReadPacket(AVPacketWrapper &packet)
{
	int ret = ::avcodec_receive_packet(_wrapped_obj, packet);
	if (!ret)
	{
		packet.SetTimeBase(TimeBase());
	}

	return ret;
}

#pragma endregion

#pragma region 解码

void video::AVCodecContextWrapper::SendPacket(AVPacketWrapper *packet)
{
	int ret;
	if (packet)
	{
		/* 解码不需要对包进行时间基调整。AVCodecContext 在作为解码器的时候自己会记录时间基，将所有
		 * 包的时间基都当作这个时间基。从封装中读出来的包不含有时间基信息（时间基字段为：0 / 1 ，是个无效值），
		 * 只含有 pst 和 dts。
		 */
		ret = avcodec_send_packet(_wrapped_obj, *packet);
	}
	else
	{
		ret = avcodec_send_packet(_wrapped_obj, nullptr);
	}

	if (ret < 0)
	{
		std::cout << CODE_POS_STR << "错误代码：" << ret << " -- " << base::ToString((ErrorCode)ret);
		// throw SendPacketException{};
	}
}

int video::AVCodecContextWrapper::ReadFrame(AVFrameWrapper &frame)
{
	// avcodec_receive_frame 内部在执行工作之前会先调用 av_frame_unref
	int ret = avcodec_receive_frame(_wrapped_obj, frame);
	if (!ret)
	{
		// 解码出来的帧会被设置时间戳，但是时间基不会被设置。这里补充设置。
		frame.SetTimeBase(TimeBase());
	}

	return ret;
}

#pragma endregion

#pragma region IAudioStreamInfoCollection

AVChannelLayout video::AVCodecContextWrapper::ChannelLayout() const
{
	return _wrapped_obj->ch_layout;
}

void video::AVCodecContextWrapper::SetChannelLayout(AVChannelLayout value)
{
	_wrapped_obj->ch_layout = value;
}

AVSampleFormat video::AVCodecContextWrapper::SampleFormat() const
{
	return _wrapped_obj->sample_fmt;
}

void video::AVCodecContextWrapper::SetSampleFormat(AVSampleFormat value)
{
	_wrapped_obj->sample_fmt = value;
}

int video::AVCodecContextWrapper::SampleRate() const
{
	return _wrapped_obj->sample_rate;
}

void video::AVCodecContextWrapper::SetSampleRate(int value)
{
	_wrapped_obj->sample_rate = value;
}

AVRational video::AVCodecContextWrapper::TimeBase() const
{
	/**
	 * 这是表示时间的基本单位（以秒为单位），帧的时间戳就是基于这个单位来表示的。对于固定帧率的内容，
	 * time_base 应该是 1/帧率，并且时间戳增量应该是相同的 1。
	 * 对于视频来说，这通常是帧率或场率的倒数，但如果帧率不是常数的话，1/time_base 并不是平均帧率。
	 *
	 * 像容器一样，基本流也可以存储时间戳，1/time_base 是这些时间戳所指定的单位。
	 * 例如，可以参考 ISO/IEC 14496-2:2001(E) 中的 vop_time_increment_resolution 和 fixed_vop_rate
	 * （fixed_vop_rate == 0 表示它与帧率不同）。
	 *
	 * - 编码时：必须由用户设置。
	 * - 解码时：不使用。
	 */
	return _wrapped_obj->time_base;
}

void video::AVCodecContextWrapper::SetTimeBase(AVRational value)
{
	_wrapped_obj->time_base = value;
}

#pragma endregion

#pragma region IVideoStreamInfoCollection

int video::AVCodecContextWrapper::Width() const
{
	return _wrapped_obj->width;
}

void video::AVCodecContextWrapper::SetWidth(int value)
{
	_wrapped_obj->width = value;
}

int video::AVCodecContextWrapper::Height() const
{
	return _wrapped_obj->height;
}

void video::AVCodecContextWrapper::SetHeight(int value)
{
	_wrapped_obj->height = value;
}

AVPixelFormat video::AVCodecContextWrapper::PixelFormat() const
{
	return _wrapped_obj->pix_fmt;
}

void video::AVCodecContextWrapper::SetPixelFormat(AVPixelFormat value)
{
	_wrapped_obj->pix_fmt = value;
}

AVRational video::AVCodecContextWrapper::FrameRate() const
{
	return _wrapped_obj->framerate;
}

void video::AVCodecContextWrapper::SetFrameRate(AVRational value)
{
	_wrapped_obj->framerate = value;
}

#pragma endregion
