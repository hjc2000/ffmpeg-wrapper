#pragma once

extern "C"
{
	#include<libavcodec/avcodec.h>
	#include<libavfilter/buffersink.h>
	#include<libavfilter/buffersrc.h>
	#include<libavformat/avformat.h>
	#include<libavformat/avio.h>
	#include<libavutil/audio_fifo.h>
	#include<libavutil/avutil.h>
	#include<libavutil/channel_layout.h>
	#include<libavutil/error.h>
	#include<libavutil/fifo.h>
	#include<libavutil/imgutils.h>
	#include<libavutil/log.h>
	#include<libavutil/opt.h>
	#include<libavutil/pixdesc.h>
	#include<libavutil/samplefmt.h>
	#include<libavutil/timestamp.h>
	#include<libswresample/swresample.h>
	#include<libswscale/swscale.h>
}

#include<base/Wrapper.h>
#include<jccpp/define.h>
#include<fstream>
#include<functional>
#include<iostream>
#include<memory>
#include<sstream>
#include<string>

using std::function;
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::fstream;
using std::ios_base;
using std::shared_ptr;
using std::unique_ptr;
