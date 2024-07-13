#include "ReencodeDotNetVideoStream.h"
#include <base/task/CancellationTokenSource.h>
#include <ffmpeg-wrapper/factory/EncoderPipeFactoryManager.h>
#include <ffmpeg-wrapper/info-collection/VideoStreamInfoCollection.h>
#include <jccpp/TaskCompletionSignal.h>

using namespace std;
using namespace video;

void ReencodeDotNetVideoStream(DotNetStream *dotnet_video_stream)
{
	// 想要输出的视频信息
	VideoStreamInfoCollection output_video_stream_infos;
	output_video_stream_infos._frame_rate = AVRational{30, 1};
	output_video_stream_infos._width = 1920;
	output_video_stream_infos._height = 1080;
	output_video_stream_infos._pixel_format = AVPixelFormat::AV_PIX_FMT_YUV420P;

	// 想要输出的音频信息
	AudioStreamInfoCollection output_audio_stream_infos;
	output_audio_stream_infos._ch_layout =
		AVChannelLayoutExtension::GetDefaultChannelLayout(2);
	output_audio_stream_infos._sample_format = AVSampleFormat::AV_SAMPLE_FMT_FLTP;
	output_audio_stream_infos._sample_rate = 48000;

	// 编码封装管道
	shared_ptr<base::Stream> out_fs = jccpp::FileStream::CreateNewAnyway("mux_out.ts");
	shared_ptr<StreamOutputFormat> out_fmt_ctx{new StreamOutputFormat{".ts", out_fs}};
	shared_ptr<SptsEncodeMux> spts_encode_mux{
		new SptsEncodeMux{
			out_fmt_ctx,
			output_video_stream_infos,
			"hevc_amf",
			-1,
			output_audio_stream_infos,
			"aac"},
	};

	// 输入格式
	shared_ptr<JoinedInputFormatDemuxDecoder> joined_input_format_demux_decoder{
		new JoinedInputFormatDemuxDecoder{}};
	joined_input_format_demux_decoder->AddVideoFrameConsumer(
		spts_encode_mux->VideoEncodePipe());
	joined_input_format_demux_decoder->AddAudioFrameConsumer(
		spts_encode_mux->AudioEncodePipe());

	int loop_times = 0;

	auto get_input_format_func = [&](shared_ptr<InputFormat> &current_input_format)
	{
		if (loop_times > 2)
		{
			return;
		}

		dotnet_video_stream->SetPosition(0);
		shared_ptr<InputFormat> in_fmt_ctx{new InputFormat{dotnet_video_stream->ToSharePtr()}};
		loop_times++;
		current_input_format = in_fmt_ctx;
	};
	joined_input_format_demux_decoder->NeedInputFormatEvent().Subscribe(get_input_format_func);

	// 解码管道
	base::CancellationTokenSource cancel_pump_source;
	TaskCompletionSignal pump_thread_exit{false};

	auto thread_fun = [&]()
	{
		try
		{
			joined_input_format_demux_decoder->PumpDataToConsumers(cancel_pump_source.Token());
		}
		catch (std::exception &e)
		{
			cerr << e.what() << endl;
		}
		catch (...)
		{
			cerr << "发生未知异常" << endl;
		}

		cout << "线程退出" << endl;
		pump_thread_exit.SetResult();
	};

	std::thread(thread_fun).detach();

	cin.get();
	cancel_pump_source.Cancel();
	pump_thread_exit.Wait();
}
