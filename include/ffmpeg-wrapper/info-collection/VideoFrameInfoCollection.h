#pragma once
#include <ffmpeg-wrapper/info-collection/IVideoFrameInfoCollection.h>

namespace video
{
    /// @brief 视频帧信息集合。
    class VideoFrameInfoCollection :
        public IVideoFrameInfoCollection
    {
    public:
        /// @brief 构造函数
        VideoFrameInfoCollection() = default;

        /// @brief 拷贝构造函数
        /// @param o
        VideoFrameInfoCollection(VideoFrameInfoCollection const &o)
        {
            IVideoFrameInfoCollection::operator=(o);
        }

        /// @brief 通过将 IVideoFrameInfoCollection 接口对象的信息拷贝过来从而构造本对象。
        /// @param o
        VideoFrameInfoCollection(IVideoFrameInfoCollection const &o)
        {
            IVideoFrameInfoCollection::operator=(o);
        }

        /// @brief 赋值运算符。
        /// @param o
        /// @return
        VideoFrameInfoCollection &operator=(VideoFrameInfoCollection const &o)
        {
            IVideoFrameInfoCollection::operator=(o);
            return *this;
        }

        int _width = 0;
        int _height = 0;
        AVPixelFormat _pixel_format{};

#pragma region IVideoFrameInfoCollection
        int Width() const override;
        void SetWidth(int value) override;

        int Height() const override;
        void SetHeight(int value) override;

        AVPixelFormat PixelFormat() const override;
        void SetPixelFormat(AVPixelFormat value) override;
#pragma endregion
    };
} // namespace video
