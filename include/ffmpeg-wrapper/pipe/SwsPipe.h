#pragma once
#include <base/container/List.h>
#include <base/pipe/IConsumer.h>
#include <base/pipe/IPipeSource.h>
#include <ffmpeg-wrapper/info-collection/VideoFrameInfoCollection.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include <ffmpeg-wrapper/wrapper/SwsContextWrapper.h>
#include <memory>

namespace video
{
	class SwsPipe :
		public base::IPipeSource<AVFrameWrapper>,
		public base::IConsumer<AVFrameWrapper>
	{
	private:
		base::List<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> _consumer_list;
		std::shared_ptr<SwsContextWrapper> _sws_context;
		VideoFrameInfoCollection _in_video_frame_infos;
		VideoFrameInfoCollection _desire_out_video_frame_infos;
		AVFrameWrapper _sws_out_frame;

		void ReadAndSendFrame();
		void ReplaceSws();

	public:
		SwsPipe(IVideoFrameInfoCollection const &desire_out_video_frame_infos);

		base::IList<std::shared_ptr<base::IConsumer<AVFrameWrapper>>> &ConsumerList() override
		{
			return _consumer_list;
		}

		void SendData(AVFrameWrapper &frame) override;
		void Flush() override;
	};
} // namespace video
