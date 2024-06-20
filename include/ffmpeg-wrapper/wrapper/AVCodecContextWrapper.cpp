#include"ffmpeg-wrapper/wrapper/AVCodecContextWrapper.h"
#include<ffmpeg-wrapper/AVCalculate.h>
#include<ffmpeg-wrapper/AVCodecExtention.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/wrapper/AVDictionaryWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include<format>

using namespace video;

#pragma region 构造,析构
AVCodecContextWrapper::AVCodecContextWrapper(AVCodec const *codec)
{
	SetCodec(codec);
	_wrapped_obj = ::avcodec_alloc_context3(codec);
	if (!_wrapped_obj)
	{
		throw std::runtime_error("AVCodecContextWrapper create(AVCodecWrapper codec) 失败");
	}
}

AVCodecContextWrapper::AVCodecContextWrapper(const AVCodec *codec, AVCodecParameters *param)
	: AVCodecContextWrapper(codec)
{
	SetCodecParams(param);
}

AVCodecContextWrapper::~AVCodecContextWrapper()
{
	avcodec_free_context(&_wrapped_obj);
}
#pragma endregion

#pragma region 工厂函数
shared_ptr<AVCodecContextWrapper> AVCodecContextWrapper::CreateDecoder(AVStreamInfoCollection stream)
{
	shared_ptr<AVCodecContextWrapper> ctx{
		new AVCodecContextWrapper{
			stream._codec,
			stream._codec_params
	}
	};

	ctx->SetTimeBase(stream.TimeBase());
	ctx->SetFrameRate(stream.FrameRate());

	ctx->Open();
	return ctx;
}

shared_ptr<AVCodecContextWrapper> AVCodecContextWrapper::CreateEncoder(
	const char *encoder_name,
	IAudioStreamInfoCollection const &infos,
	bool set_global_header,
	bool auto_open
)
{
	auto codec = AVCodecExtention::find_encoder_by_name(encoder_name);
	if (!codec)
	{
		throw std::runtime_error{ CODE_POS_STR + std::string{"查找编码器失败"} };
	}

	shared_ptr<AVCodecContextWrapper> ctx{ new AVCodecContextWrapper{codec,} };

	// 设置编码器参数
	(*ctx)->codec_type = AVMediaType::AVMEDIA_TYPE_AUDIO;
	(*ctx)->ch_layout = infos.ChannelLayout();
	(*ctx)->sample_fmt = infos.SampleFormat();
	(*ctx)->sample_rate = infos.SampleRate();
	(*ctx)->time_base = infos.TimeBase();
	if (set_global_header)
	{
		ctx->SetGlobalHeader();
	}

	if (auto_open)
	{
		ctx->Open();
	}

	return ctx;
}

shared_ptr<AVCodecContextWrapper> AVCodecContextWrapper::CreateEncoder(
	const char *encoder_name,
	IVideoStreamInfoCollection const &infos,
	bool set_global_header,
	bool auto_open
)
{
	auto codec = AVCodecExtention::find_encoder_by_name(encoder_name);
	if (!codec)
	{
		throw std::runtime_error{ CODE_POS_STR + std::string{"查找编码器失败"} };
	}

	shared_ptr<AVCodecContextWrapper> ctx{ new AVCodecContextWrapper{codec} };

	// 设置编码器参数
	(*ctx)->codec_type = AVMediaType::AVMEDIA_TYPE_VIDEO;

	(*ctx)->width = infos.Width();
	(*ctx)->height = infos.Height();
	(*ctx)->pix_fmt = infos.PixelFormat();

	(*ctx)->time_base = infos.TimeBase();
	(*ctx)->framerate = infos.FrameRate();

	(*ctx)->gop_size = infos.FrameRate().num / infos.FrameRate().den;
	//(*ctx)->max_b_frames = 10;
	if (set_global_header)
	{
		ctx->SetGlobalHeader();
	}

	if (auto_open)
	{
		ctx->Open();
	}

	return ctx;
}
#pragma endregion

void AVCodecContextWrapper::SetCodecParams(AVCodecParameters *param)
{
	int ret = ::avcodec_parameters_to_context(_wrapped_obj, param);
	if (ret < 0)
	{
		throw std::runtime_error("AVCodecContextWrapper::set_encoder_param");
	}
}

void AVCodecContextWrapper::Open(AVDictionary **dic)
{
	int ret = ::avcodec_open2(_wrapped_obj, _codec, dic);
	if (ret)
	{
		throw std::runtime_error{ CODE_POS_STR + std::string{"打开编解码器失败"} };
	}
}

#pragma region 编码
void AVCodecContextWrapper::SendFrame(AVFrameWrapper *frame)
{
	int ret;
	if (frame)
	{
		// 正常送入帧
		if (frame->TimeBase() != TimeBase())
		{
			frame->ChangeTimeBase(TimeBase());
		}

		frame->make_writable();
		ret = ::avcodec_send_frame(_wrapped_obj, *frame);
	}
	else
	{
		// 送入空指针，表示冲洗编码器
		ret = ::avcodec_send_frame(_wrapped_obj, nullptr);
	}

	if (ret)
	{
		std::string msg = std::format(
			"送入帧失败，错误代码：{} —— {}",
			ret,
			ToString((ErrorCode)ret)
		);

		throw std::runtime_error{ CODE_POS_STR + msg };
	}
}

int AVCodecContextWrapper::ReadPacket(AVPacketWrapper &packet)
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
void AVCodecContextWrapper::SendPacket(AVPacketWrapper *packet)
{
	int ret;
	if (packet)
	{
		/* 解码不需要对包进行时间基调整。AVCodecContext 在作为解码器的时候自己会记录时间基，将所有
		* 包的时间基都当作这个时间基。从封装中读出来的包不含有时间基信息（时间基字段为：0 / 1 ，是个无效值），
		* 只含有 pst 和 dts。
		*/
		ret = ::avcodec_send_packet(_wrapped_obj, *packet);
	}
	else
	{
		ret = ::avcodec_send_packet(_wrapped_obj, nullptr);
	}

	if (ret < 0)
	{
		cout << CODE_POS_STR << "错误代码：" << ret << " -- " << ToString((ErrorCode)ret);
		//throw SendPacketException{};
	}
}

int AVCodecContextWrapper::ReadFrame(AVFrameWrapper &frame)
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

#pragma region IAudioStreamInfoCollection, IVideoStreamInfoCollection
AVChannelLayout AVCodecContextWrapper::ChannelLayout() const
{
	return _wrapped_obj->ch_layout;
}

void AVCodecContextWrapper::SetChannelLayout(AVChannelLayout value)
{
	_wrapped_obj->ch_layout = value;
}

AVSampleFormat AVCodecContextWrapper::SampleFormat() const
{
	return _wrapped_obj->sample_fmt;
}

void AVCodecContextWrapper::SetSampleFormat(AVSampleFormat value)
{
	_wrapped_obj->sample_fmt = value;
}

int AVCodecContextWrapper::SampleRate() const
{
	return _wrapped_obj->sample_rate;
}

void AVCodecContextWrapper::SetSampleRate(int value)
{
	_wrapped_obj->sample_rate = value;
}

int AVCodecContextWrapper::Width() const
{
	return _wrapped_obj->width;
}
void AVCodecContextWrapper::SetWidth(int value)
{
	_wrapped_obj->width = value;
}

int AVCodecContextWrapper::Height() const
{
	return _wrapped_obj->height;
}
void AVCodecContextWrapper::SetHeight(int value)
{
	_wrapped_obj->height = value;
}

AVPixelFormat AVCodecContextWrapper::PixelFormat() const
{
	return _wrapped_obj->pix_fmt;
}

void AVCodecContextWrapper::SetPixelFormat(AVPixelFormat value)
{
	_wrapped_obj->pix_fmt = value;
}

AVRational AVCodecContextWrapper::TimeBase() const
{
	return _wrapped_obj->time_base;
}

void AVCodecContextWrapper::SetTimeBase(AVRational value)
{
	_wrapped_obj->time_base = value;
}

AVRational AVCodecContextWrapper::FrameRate() const
{
	return _wrapped_obj->framerate;
}

void AVCodecContextWrapper::SetFrameRate(AVRational value)
{
	_wrapped_obj->framerate = value;
}
#pragma endregion
