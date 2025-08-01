#include "SwrContextWrapper.h"
#include <ffmpeg-wrapper/AVCalculate.h>
#include <ffmpeg-wrapper/AVCompare.h>

using namespace video;

SwrContextWrapper::SwrContextWrapper(IAudioStreamInfoCollection const &in_stream_infos,
									 IAudioFrameInfoCollection const &out_frame_infos)
{
	_in_stream_infos = in_stream_infos;
	_out_frame_infos = out_frame_infos;

	int ret = swr_alloc_set_opts2(&_wrapped_obj,
								  &_out_frame_infos._ch_layout,
								  _out_frame_infos._sample_format,
								  _out_frame_infos._sample_rate,
								  &_in_stream_infos._ch_layout,
								  _in_stream_infos._sample_format,
								  _in_stream_infos._sample_rate,
								  0,
								  nullptr);

	if (ret < 0)
	{
		throw std::runtime_error{
			CODE_POS_STR + std::string{"构造重采样器并设置参数失败"},
		};
	}

	ret = swr_init(_wrapped_obj);
	if (ret < 0)
	{
		throw std::runtime_error{
			CODE_POS_STR + std::string{"初始化重采样器失败"},
		};
	}
}

SwrContextWrapper::~SwrContextWrapper()
{
	swr_free(&_wrapped_obj);
}

void video::SwrContextWrapper::SendData(AVFrameWrapper &input_frame)
{
	_in_pts_when_send_frame = base::Fraction{input_frame.Pts()} *
							  video::ToFraction(input_frame.TimeBase()) /
							  base::Fraction{1, 90000};

	// 非冲洗模式
	if (input_frame.TimeBase() != _in_stream_infos.TimeBase())
	{
		input_frame.ChangeTimeBase(_in_stream_infos.TimeBase());
	}

	int ret = swr_convert(_wrapped_obj,
						  nullptr,
						  0,
						  input_frame->extended_data,
						  input_frame.SampleCount());

	if (ret < 0)
	{
		throw std::runtime_error{
			base::ToString(static_cast<ErrorCode>(ret)),
		};
	}
}

void video::SwrContextWrapper::Flush()
{
	// 冲洗模式
	_flushed = true;

	int ret = swr_convert(_wrapped_obj,
						  nullptr,
						  0,
						  nullptr,
						  0);

	if (ret < 0)
	{
		throw std::runtime_error{
			base::ToString(static_cast<ErrorCode>(ret)),
		};
	}
}

bool video::SwrContextWrapper::ReadData(AVFrameWrapper &output_frame)
{
	output_frame = AVFrameWrapper{_out_frame_infos};

	int ret = 0;
	if (_flushed)
	{
		// 冲洗模式
		ret = ReadFrameInFlushingMode(output_frame);
	}
	else
	{
		// 非冲洗模式
		ret = ReadFrameInNonFlushingMode(output_frame);
	}

	/* 在送入帧的时候记录了当时的 pts，为 _pts_when_send_frame。现在已经读取出 output_frame 了，
	 * 将 _in_pts_when_send_frame 减去重采样器内，以输入侧时间基为时间基的延迟，就可以得到 output_frame
	 * 的 pts 在输入侧的值。因为延迟的关系，它比 _in_pts_when_send_frame 小。
	 *
	 * 但是，in_pts 是输入侧的时间戳，我们需要转换为在输出侧的时间戳，然后赋值给 output_frame。
	 */
	base::Fraction delay = GetDelay(90000);
	if (ret == static_cast<int>(ErrorCode::eof))
	{
		/* 不清楚冲洗完后重采样器内会不会仍然延迟不为 0，所以冲洗后，并且返回 eof，此时表示重采样器空了。
		 * 手动将 delay 设为 0.
		 */
		delay = 0;
	}

	base::Fraction out_pts = (_in_pts_when_send_frame - delay) *
							 base::Fraction{1, 90000} /
							 video::ToFraction(_out_frame_infos.TimeBase());

	output_frame.SetPts(static_cast<int64_t>(out_pts));
	output_frame.SetTimeBase(_out_frame_infos.TimeBase());
	return ret >= 0;
}

int SwrContextWrapper::ReadFrameInFlushingMode(AVFrameWrapper &output_frame)
{
	int count = swr_convert(_wrapped_obj,
							output_frame->extended_data,
							output_frame.SampleCount(),
							nullptr,
							0);

	// 如果有填充（即 count > 0）且填充了不完整的帧
	if (count > 0 && count < output_frame.SampleCount())
	{
		// 将后面没被填充的采样点设置为静音
		output_frame.Mute(count);
	}

	if (count > 0)
	{
		/* 有填充，说明重采样器内还是有采样点的，返回 0，表示读取成功。
		 * 只不过这个读取成功的帧有可能尾部是被填充了静音采样点的。
		 */
		return 0;
	}

	// 一个采样点都没填充，本次读取帧失败，返回 eof。
	return static_cast<int>(ErrorCode::eof);
}

int SwrContextWrapper::ReadFrameInNonFlushingMode(AVFrameWrapper &output_frame)
{
	if (CanFillOutputFrame(output_frame))
	{
		// 可以填充一个完整的帧
		int count = swr_convert(_wrapped_obj,
								output_frame->extended_data,
								output_frame.SampleCount(),
								nullptr,
								0);

		if (count != output_frame.SampleCount())
		{
			throw std::runtime_error{"read_frame 没有填充完整的 output_frame，本来认为这里一定会填充完整的帧的。"};
		}

		return 0;
	}

	// 无法填充一个完整的帧，所以一个采样点都不填充
	// 返回 -11 表示输出暂时不可用，需要更多的输入数据。这是官方的错误代码。
	return static_cast<int>(ErrorCode::output_is_temporarily_unavailable);
}

int SwrContextWrapper::AvaliableSampleCount(int in_nb_samples)
{
	int samples = swr_get_out_samples(_wrapped_obj, in_nb_samples);
	if (samples < 0)
	{
		throw std::runtime_error{"swr_get_out_samples 函数出错"};
	}

	return samples;
}

int64_t SwrContextWrapper::GetDelay(int64_t base)
{
	return swr_get_delay(_wrapped_obj, base);
}

int SwrContextWrapper::GetDelayAsSampleCount()
{
	// GetDelay 已经加锁了，这里不用加锁。
	return static_cast<int>(GetDelay(_out_frame_infos._sample_rate));
}
