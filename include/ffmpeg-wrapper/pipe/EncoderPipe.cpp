#include "EncoderPipe.h"
#include <ffmpeg-wrapper/ErrorCode.h>

void video::EncoderPipe::ReadAndSendPacketToOutputFormat()
{
	AVPacketWrapper packet;
	while (true)
	{
		int ret = _encoder_ctx->ReadPacket(packet);
		switch (ret)
		{
		case 0:
			{
				packet.SetStreamIndex(_new_stream.Index());
				_output_format->SendData(packet);

				// 退出 switch，下一轮循环继续读取包
				break;
			}
		case (int)ErrorCode::output_is_temporarily_unavailable:
			{
				return;
			}
		case (int)ErrorCode::eof:
			{
				_output_format->Flush();
				return;
			}
		default:
			{
				throw std::runtime_error{CODE_POS_STR + std::string{"ReadData 返回了未知的错误代码"}};
			}
		}
	}
}

video::EncoderPipe::EncoderPipe(std::string codec_name,
								IVideoStreamInfoCollection const &stream_infos,
								std::shared_ptr<OutputFormat> output_format,
								int64_t out_bit_rate_in_bps)
{
	_output_format = output_format;
	_encoder_ctx = AVCodecContextWrapper::CreateEncoder(codec_name.c_str(),
														stream_infos,
														_output_format->NeedGlobalHeader(),
														false);

	if (out_bit_rate_in_bps > 0)
	{
		// 如果传入了有效值，则设置 bit_rate 字段。
		(*_encoder_ctx)->bit_rate = out_bit_rate_in_bps;
	}

	//// 设置色彩空间为 Rec.2020
	//(*_encoder_ctx)->colorspace = AVCOL_SPC_BT2020_NCL;

	//// 色彩原色设置
	//(*_encoder_ctx)->color_primaries = AVCOL_PRI_BT2020;

	//// 转移特性，例如使用 PQ
	//(*_encoder_ctx)->color_trc = AVCOL_TRC_SMPTE2084;

	_encoder_ctx->Open();
	_new_stream = _output_format->CreateNewStream(*_encoder_ctx);
}

video::EncoderPipe::EncoderPipe(std::string codec_name,
								IAudioStreamInfoCollection const &in_stream_infos,
								std::shared_ptr<OutputFormat> output_format)
{
	_output_format = output_format;
	_encoder_ctx = AVCodecContextWrapper::CreateEncoder(codec_name.c_str(),
														in_stream_infos,
														_output_format->NeedGlobalHeader());

	_new_stream = _output_format->CreateNewStream(*_encoder_ctx);
}

void video::EncoderPipe::SendData(AVFrameWrapper &frame)
{
	// 防止编码器中有数据残留
	ReadAndSendPacketToOutputFormat();
	_encoder_ctx->SendFrame(&frame);
	ReadAndSendPacketToOutputFormat();
}

void video::EncoderPipe::Flush()
{
	ReadAndSendPacketToOutputFormat();
	_encoder_ctx->SendFrame(nullptr);
	ReadAndSendPacketToOutputFormat();
}
