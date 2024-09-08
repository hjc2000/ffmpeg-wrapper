#pragma once
#include <base/Wrapper.h>
#include <ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include <memory>

namespace video
{
    class AVCodecWrapper;
    class AVCodecContextWrapper;

    /// @brief AVStream 包装器
    /// @note AVStream 的生命周期是 AVFormat 管理的，所以本类指示包装它，不拥有它，
    /// 析构时不会释放被包装的 AVStream 对象。拷贝构造时不会深度拷贝，仅仅是
    /// 将被包装对象的指针拷贝。
    class AVStreamWrapper :
        public base::Wrapper<AVStream>,
        public IAudioStreamInfoCollection,
        public IVideoStreamInfoCollection
    {
    private:
        AVStream *_wrapped_obj = nullptr;

    public:
        AVStreamWrapper();
        AVStreamWrapper(AVStreamWrapper const &o);
        AVStreamWrapper(AVStream *p);
        AVStreamWrapper &operator=(AVStreamWrapper const &o);

        AVStream *&WrappedObj() override
        {
            return _wrapped_obj;
        }

        AVStream *WrappedObj() const override
        {
            return _wrapped_obj;
        }

        /// @brief 获取本流的编解码器参数
        /// @return
        AVCodecParameters &CodecParams() const;

        /// @brief 设置本流的编解码器参数。
        /// @param params
        void SetCodecParams(AVCodecParameters const &params);

        /// @brief 通过 AVCodecContextWrapper 设置此流信息。
        /// @note 除了会设置本流的 AVCodecParameters 外，还会额外设置时间基和帧率。
        /// 如果本流是音频流，帧率是没用的，但是设置了也无伤大雅。
        /// @param codec_ctx 此码器上下文的信息将被复制到此流中。
        void SetCodecParams(AVCodecContextWrapper const &codec_ctx);

        /// @brief 获取此流对应的编解码器
        /// @return 找不到会返回空指针。
        AVCodec const *Codec() const;

        int64_t Bitrate() const;
        AVMediaType MediaType() const;

        /// @brief 获取本流在格式中的索引值
        /// @return
        int Index() const;
        void SetIndex(int value);

#pragma region IAudioStreamInfoCollection
        AVSampleFormat SampleFormat() const override;
        void SetSampleFormat(AVSampleFormat value) override;

        AVChannelLayout ChannelLayout() const override;
        void SetChannelLayout(AVChannelLayout value) override;

        int SampleRate() const override;
        void SetSampleRate(int value) override;

        AVRational TimeBase() const override;
        void SetTimeBase(AVRational value) override;
#pragma endregion

#pragma region IVideoStreamInfoCollection
        int Width() const override;
        void SetWidth(int value) override;

        int Height() const override;
        void SetHeight(int value) override;

        AVPixelFormat PixelFormat() const override;
        void SetPixelFormat(AVPixelFormat value) override;

        AVRational FrameRate() const override;
        void SetFrameRate(AVRational value) override;
#pragma endregion
    };
} // namespace video
