#pragma once
#include <base/Wrapper.h>
#include <ffmpeg-wrapper/AVCalculate.h>
#include <ffmpeg-wrapper/base_include.h>

namespace video
{
    class AVFrameWrapper;
    class AVCodecContextWrapper;

    class AVPacketWrapper : public base::Wrapper<AVPacket>
    {
    private:
        AVPacket *_wrapped_obj = nullptr;

#pragma region 私有生命周期
        /// @brief 让本包引用另一个包的缓冲区，并且复制其它参数。
        /// 在引用另一个包之前会先调用 Unref 方法。
        ///
        /// @param other
        void Ref(AVPacketWrapper const &other);

        /// @brief 解除此包对缓冲区的引用
        void Unref();
#pragma endregion

    public:
        AVPacketWrapper();
        AVPacketWrapper(AVPacketWrapper const &o);
        ~AVPacketWrapper();
        AVPacketWrapper &operator=(AVPacketWrapper const &o);

        AVPacket *&WrappedObj() override
        {
            return _wrapped_obj;
        }

        AVPacket *WrappedObj() const override
        {
            return _wrapped_obj;
        }

        void ChangeTimeBase(AVRational new_time_base);

        int StreamIndex() const;
        void SetStreamIndex(int value);

        int64_t Duration() const;
        void SetDuration(int64_t value);

        int64_t Pts() const;
        void SetPts(int64_t value);

        /// @brief 包的 dts
        /// @note 包最好以 dts 为准，因为如果视频含有 B 帧，pts 可能会出现无效值。
        ///
        /// @return
        int64_t Dts() const;
        void SetDts(int64_t value);

        AVRational TimeBase() const;
        void SetTimeBase(AVRational value);
    };
} // namespace video
