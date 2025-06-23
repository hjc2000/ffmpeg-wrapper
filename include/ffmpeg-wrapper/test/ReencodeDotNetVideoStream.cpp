#include "ReencodeDotNetVideoStream.h"
#include <base/task/CancellationTokenSource.h>
#include <base/task/TaskCompletionSignal.h>
#include <ffmpeg-wrapper/AVChannelLayoutExtension.h>
#include <ffmpeg-wrapper/factory/EncoderPipeFactoryManager.h>
#include <ffmpeg-wrapper/info-collection/VideoStreamInfoCollection.h>

void ReencodeDotNetVideoStream(DotNetStream *dotnet_video_stream)
{
	// 想要输出的视频信息
	video::VideoStreamInfoCollection output_video_stream_infos;
	output_video_stream_infos._frame_rate = AVRational{30, 1};
	output_video_stream_infos._width = 1920;
	output_video_stream_infos._height = 1080;
	output_video_stream_infos._pixel_format = AVPixelFormat::AV_PIX_FMT_YUV420P;

	// 想要输出的音频信息
	video::AudioStreamInfoCollection output_audio_stream_infos;
	output_audio_stream_infos._ch_layout = video::AVChannelLayoutExtension::GetDefaultChannelLayout(2);
	output_audio_stream_infos._sample_format = AVSampleFormat::AV_SAMPLE_FMT_FLTP;
	output_audio_stream_infos._sample_rate = 48000;

	// 编码封装管道
	std::shared_ptr<base::Stream> out_fs = base::file::CreateNewAnyway("mux_out.ts");

	std::shared_ptr<video::StreamOutputFormat> out_fmt_ctx{new video::StreamOutputFormat{
		".ts",
		out_fs,
	}};

	std::shared_ptr<video::SptsEncodeMux> spts_encode_mux{new video::SptsEncodeMux{
		out_fmt_ctx,
		output_video_stream_infos,
		"hevc_amf",
		-1,
		output_audio_stream_infos,
		"aac",
	}};

	// 输入格式
	std::shared_ptr<video::JoinedInputFormatDemuxDecoder> joined_input_format_demux_decoder{new video::JoinedInputFormatDemuxDecoder{}};
	joined_input_format_demux_decoder->AddVideoFrameConsumer(spts_encode_mux->VideoEncodePipe());
	joined_input_format_demux_decoder->AddAudioFrameConsumer(spts_encode_mux->AudioEncodePipe());

	int loop_times = 0;

	joined_input_format_demux_decoder->NeedInputFormatEvent().Subscribe(
		[&](std::shared_ptr<video::InputFormat> &current_input_format)
		{
			if (loop_times > 2)
			{
				return;
			}

			dotnet_video_stream->SetPosition(0);
			std::shared_ptr<video::InputFormat> in_fmt_ctx{new video::InputFormat{dotnet_video_stream->ToSharePtr()}};
			loop_times++;
			current_input_format = in_fmt_ctx;
		});

	// 解码管道
	base::CancellationTokenSource cancel_pump_source;
	base::task::TaskCompletionSignal pump_thread_exit{false};

	std::thread{
		[&]()
		{
			try
			{
				joined_input_format_demux_decoder->PumpDataToConsumers(cancel_pump_source.Token());
			}
			catch (std::exception &e)
			{
				std::cerr << e.what() << std::endl;
			}
			catch (...)
			{
				std::cerr << "发生未知异常" << std::endl;
			}

			std::cout << "线程退出" << std::endl;
			pump_thread_exit.SetResult();
		}}
		.detach();

	std::cin.get();
	cancel_pump_source.Cancel();
	pump_thread_exit.Wait();
}
