#pragma once
#include <ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include <memory>

namespace video
{
    /// @brief 视频流信息集合。
    class VideoStreamInfoCollection :
        public IVideoStreamInfoCollection
    {
    public:
        VideoStreamInfoCollection() = default;

        VideoStreamInfoCollection(VideoStreamInfoCollection const &o)
        {
            IVideoStreamInfoCollection::operator=(o);
        }

        VideoStreamInfoCollection(IVideoStreamInfoCollection const &o)
        {
            IVideoStreamInfoCollection::operator=(o);
        }

        VideoStreamInfoCollection(std::shared_ptr<IVideoStreamInfoCollection> value)
        {
            IVideoStreamInfoCollection::operator=(*value);
        }

        VideoStreamInfoCollection &operator=(IVideoStreamInfoCollection const &value)
        {
            IVideoStreamInfoCollection::operator=(value);
            return *this;
        }

        int _width = 0;
        int _height = 0;
        AVPixelFormat _pixel_format = AVPixelFormat{};
        AVRational _frame_rate = AVRational{};
        AVRational _time_base = AVRational{};

#pragma region IVideoStreamInfoCollection

        int Width() const override
        {
            return _width;
        }

        void SetWidth(int value) override
        {
            _width = value;
        }

        int Height() const override
        {
            return _height;
        }

        void SetHeight(int value) override
        {
            _height = value;
        }

        AVPixelFormat PixelFormat() const override
        {
            return _pixel_format;
        }

        void SetPixelFormat(AVPixelFormat value) override
        {
            _pixel_format = value;
        }

        AVRational FrameRate() const override
        {
            return _frame_rate;
        }

        void SetFrameRate(AVRational value) override
        {
            _frame_rate = value;
        }

        virtual AVRational TimeBase() const override
        {
            return _time_base;
        }

        virtual void SetTimeBase(AVRational value) override
        {
            _time_base = value;
        }

#pragma endregion
    };
} // namespace video
