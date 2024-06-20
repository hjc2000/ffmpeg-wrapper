#pragma once
#include<ffmpeg-wrapper/info-collection/VideoFrameInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPipeFrameSource.h>
#include<ffmpeg-wrapper/wrapper/SwsContextWrapper.h>

namespace video
{
	class SwsPipe :
		public IPipeFrameSource,
		public IFrameConsumer
	{
	private:
		base::List<shared_ptr<IFrameConsumer>> _consumer_list;
		shared_ptr<SwsContextWrapper> _sws_context;
		VideoFrameInfoCollection _in_video_frame_infos;
		VideoFrameInfoCollection _desire_out_video_frame_infos;
		AVFrameWrapper _sws_out_frame;

		void ReadAndSendFrame();
		void change_sws();

	public:
		SwsPipe(IVideoFrameInfoCollection const &desire_out_video_frame_infos);

		base::List<shared_ptr<IFrameConsumer>> &FrameConsumerList() override
		{
			return _consumer_list;
		}

		void SendFrame(AVFrameWrapper *frame) override;
	};
}