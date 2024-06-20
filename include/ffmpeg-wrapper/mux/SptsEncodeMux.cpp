#include"ffmpeg-wrapper/mux/SptsEncodeMux.h"
#include<base/container/Queue.h>
#include<iostream>

using namespace video;
using namespace std;

video::SptsEncodeMux::SptsEncodeMux(
	std::shared_ptr<video::IEncoderPipeFactory> factory,
	shared_ptr<OutputFormat> out_format,
	// 视频相关参数
	IVideoStreamInfoCollection const &video_stream_infos,
	std::string video_codec_name,
	int64_t video_out_bitrate_in_bps,
	// 音频相关参数
	IAudioStreamInfoCollection const &audio_stream_infos,
	std::string audio_codec_name
)
{
	_factory = factory;
	_out_format = out_format;

	// 视频参数
	_video_stream_infos = video_stream_infos;
	_video_codec_name = video_codec_name;
	_video_out_bitrate_in_bps = video_out_bitrate_in_bps;

	// 音频参数
	_audio_stream_infos = audio_stream_infos;
	_audio_codec_name = audio_codec_name;

	// ts 必须使用 1/90000 时间基
	_video_stream_infos.SetTimeBase(AVRational { 1, 90000 });
	_audio_stream_infos.SetTimeBase(AVRational { 1, 90000 });

	InitVideoEncodePipe();
	InitAudioEncodePipe();
	WriteHeader();
}

void video::SptsEncodeMux::InitVideoEncodePipe()
{
	_video_encoder_pipe = shared_ptr<SwsFpsEncoderPipe> { new SwsFpsEncoderPipe {
		_factory,
		_out_format,
		_video_stream_infos,
		_video_codec_name,
		_video_out_bitrate_in_bps,
	} };
}

void video::SptsEncodeMux::InitAudioEncodePipe()
{
	_audio_encode_pipe = shared_ptr<SwrEncoderPipe> { new SwrEncoderPipe {
		_factory,
		_audio_codec_name,
		_audio_stream_infos,
		_out_format,
	} };
}

void video::SptsEncodeMux::WriteHeader()
{
	AVDictionaryWrapper option_dic;
	option_dic.SetValueByKey("mpegts_flags", "resend_headers");
	option_dic.SetValueByKey("pat_period ", "1000");
	option_dic.SetValueByKey("pmt_period", "1000");
	option_dic.SetValueByKey("sdt_period", "1000");
	_out_format->WriteHeader(option_dic);
	_out_format->DumpFormat();
}

shared_ptr<IFrameConsumer> video::SptsEncodeMux::VideoEncodePipe()
{
	return _video_encoder_pipe;
}

shared_ptr<IFrameConsumer> video::SptsEncodeMux::AudioEncodePipe()
{
	return _audio_encode_pipe;
}


#include<base/container/List.h>
#include<ffmpeg-wrapper/input-format/InputFormat.h>
#include<jccpp/stream/FileStream.h>
#include<ffmpeg-wrapper/demux/JoinedInputFormatDemuxDecoder.h>
#include<ffmpeg-wrapper/output-format/StreamOutputFormat.h>
#include<base/task/CancellationTokenSource.h>
#include<ffmpeg-wrapper/factory/EncoderPipeFactory.h>

/// <summary>
///		测试函数
/// </summary>
void test_SptsEncodeMux()
{
	base::Queue<std::string> file_queue;
	file_queue.Enqueue("予你成歌.ts");
	file_queue.Enqueue("越权访问.mkv");
	file_queue.Enqueue("moon.mp4");
	file_queue.Enqueue("fallen-down.ts");
	shared_ptr<JoinedInputFormatDemuxDecoder> joined_input_format_demux_decoder { new JoinedInputFormatDemuxDecoder { } };
	joined_input_format_demux_decoder->_get_format_callback = [&]()->shared_ptr<InputFormat>
	{
		try
		{
			std::string file = file_queue.Dequeue();
			shared_ptr<InputFormat> in_fmt_ctx { new InputFormat { file } };
			in_fmt_ctx->DumpFormat();
			return in_fmt_ctx;
		}
		catch (std::runtime_error &e)
		{

		}
		catch (std::exception &e)
		{
			cerr << CODE_POS_STR << e.what() << endl;
		}
		catch (...)
		{
			cerr << CODE_POS_STR << "发生未知异常" << endl;
		}

		return nullptr;
	};

	// 想要输出的视频信息
	VideoStreamInfoCollection output_video_stream_infos;
	output_video_stream_infos._frame_rate = AVRational { 30, 1 };
	output_video_stream_infos._width = 1920;
	output_video_stream_infos._height = 1080;
	output_video_stream_infos._pixel_format = AVPixelFormat::AV_PIX_FMT_YUV420P;

	// 想要输出的音频信息
	AudioStreamInfoCollection output_audio_stream_infos;
	output_audio_stream_infos._ch_layout = AVChannelLayoutExtension::GetDefaultChannelLayout(2);
	output_audio_stream_infos._sample_format = AVSampleFormat::AV_SAMPLE_FMT_FLTP;
	output_audio_stream_infos._sample_rate = 48000;

	shared_ptr<base::Stream> out_fs = jccpp::FileStream::CreateNewAnyway("mux_out.ts");
	shared_ptr<StreamOutputFormat> out_fmt_ctx { new StreamOutputFormat { ".ts", out_fs } };
	shared_ptr<SptsEncodeMux> spts_encode_mux { new SptsEncodeMux {
		video::EncoderPipeFactory::Instance(),
		out_fmt_ctx,
		output_video_stream_infos,
		"hevc_amf",
		-1,
		output_audio_stream_infos,
		"eac3"
	} };

	joined_input_format_demux_decoder->AddVideoFrameConsumer(spts_encode_mux->VideoEncodePipe());
	joined_input_format_demux_decoder->AddAudioFrameConsumer(spts_encode_mux->AudioEncodePipe());

	base::CancellationTokenSource cancel_pump_source;
	TaskCompletionSignal pump_thread_exit { false };
	std::thread([&]()
	{
		try
		{
			joined_input_format_demux_decoder->Pump(cancel_pump_source.Token());
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
	}).detach();

	cin.get();
	cancel_pump_source.Cancel();
	pump_thread_exit.Wait();
}
