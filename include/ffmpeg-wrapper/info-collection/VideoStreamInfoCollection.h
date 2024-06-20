#pragma once
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>

namespace video
{
	class VideoStreamInfoCollection :public IVideoStreamInfoCollection
	{
	public:
		VideoStreamInfoCollection() = default;

		VideoStreamInfoCollection(IVideoStreamInfoCollection const &another)
		{
			*this = another;
		}

		VideoStreamInfoCollection &operator=(shared_ptr<IVideoStreamInfoCollection> value)
		{
			*this = *value;
			return *this;
		}

		VideoStreamInfoCollection &operator=(IVideoStreamInfoCollection const &value)
		{
			IVideoStreamInfoCollection::operator=(value);
			return *this;
		}

		int _width = 0;
		int Width() const override
		{
			return _width;
		}
		void SetWidth(int value) override
		{
			_width = value;
		}

		int _height = 0;
		int Height() const override
		{
			return _height;
		}
		void SetHeight(int value) override
		{
			_height = value;
		}

		AVPixelFormat _pixel_format = AVPixelFormat{};
		AVPixelFormat PixelFormat() const override
		{
			return _pixel_format;
		}
		void SetPixelFormat(AVPixelFormat value) override
		{
			_pixel_format = value;
		}

		AVRational _frame_rate = AVRational{};
		AVRational FrameRate() const override
		{
			return _frame_rate;
		}
		void SetFrameRate(AVRational value) override
		{
			_frame_rate = value;
		}

		AVRational _time_base = AVRational{};
		virtual AVRational TimeBase() const override
		{
			return _time_base;
		}
		virtual void SetTimeBase(AVRational value) override
		{
			_time_base = value;
		}
	};
}