#include "ffmpeg-wrapper/signal-source/ISignalSource.h"
#include<ffmpeg-wrapper/wrapper/AVFrameWrapper.h>

int video::ISignalSource::ReadFrame(shared_ptr<AVFrameWrapper> frame)
{
	if (frame->IsPlanar())
	{
		throw std::invalid_argument("传进来的帧的声道布局必须是交织类型的");
	}

	if (frame->SampleFormat() != AVSampleFormat::AV_SAMPLE_FMT_DBL)
	{
		throw std::invalid_argument("传进来的帧的采样格式与信号源的期望格式不匹配");
	}

	double *channel_buffer = (double *)(*frame)->extended_data[0];
	int nb_channels = frame->ChannelLayout().nb_channels;

	// 一个音频帧有 nb_samples 个采样值
	for (int i = 0; i < frame->SampleCount(); i++)
	{
		// 采样一次
		double amp = sample();

		// 为每个声道填充相同的值
		for (int j = 0; j < nb_channels; j++)
		{
			channel_buffer[j] = amp;
		}

		/* 使指针移动一个偏移量
		*
		* 如果传进来的帧的声道布局是交织类型的，则不同声道的采样值是连续放在 extended_data[0] 这个一维数组中的。
		* 假如 extended_data[0] 中存放的是左右双声道的数据，则它的数据布局是：LRLRLR...... 这样交替的。采样一次后，
		* 为不同声道填充相同的数据，一共有 nb_channels 个声道。所以采样一次，就要让指针
		* 指针偏移 nb_samples
		*/
		channel_buffer += nb_channels;
	}

	return 0;
}
