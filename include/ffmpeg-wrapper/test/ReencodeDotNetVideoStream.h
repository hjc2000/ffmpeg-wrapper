#pragma once
#include<base/container/List.h>
#include<base/container/Queue.h>
#include<ffmpeg-wrapper/demux/JoinedInputFormatDemuxDecoder.h>
#include<ffmpeg-wrapper/input-format/InputFormat.h>
#include<ffmpeg-wrapper/mux/SptsEncodeMux.h>
#include<ffmpeg-wrapper/output-format/StreamOutputFormat.h>
#include<ffmpeg-wrapper/pipe/PacketPump.h>
#include<iostream>
#include<jccpp/stream/FileStream.h>
#include<pinvoke/DotNetStream.h>

extern "C"
{
	/// <summary>
	///		C# 打开一个文件流，传过来进行重新编码。
	/// </summary>
	/// <param name="dotnet_video_stream"></param>
	void ReencodeDotNetVideoStream(DotNetStream *dotnet_video_stream);
}
