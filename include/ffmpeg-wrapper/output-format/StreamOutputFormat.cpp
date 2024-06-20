#include"ffmpeg-wrapper/output-format/StreamOutputFormat.h"

using namespace video;
using namespace std;

shared_ptr<AVIOContextWrapper> CreateIOContext(shared_ptr<base::Stream> output_stream)
{
	shared_ptr<AVIOContextWrapper> output_context { new AVIOContextWrapper { true, output_stream } };
	return output_context;
}

StreamOutputFormat::StreamOutputFormat(std::string url, shared_ptr<base::Stream> output_stream) :
	IOContextOutputFormat(url, CreateIOContext(output_stream))
{

}
