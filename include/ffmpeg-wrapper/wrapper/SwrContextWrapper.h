#pragma once
#include <atomic>
#include <base/pipe/IConsumer.h>
#include <base/pipe/ISource.h>
#include <ffmpeg-wrapper/base_include.h>
#include <ffmpeg-wrapper/info-collection/AudioFrameInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/AudioStreamInfoCollection.h>
#include <ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include <ffmpeg-wrapper/wrapper/AVFrameWrapper.h>

namespace video
{
    /// @brief 重采样器。
    class SwrContextWrapper :
        public base::Wrapper<SwrContext>,
        public base::IConsumer<AVFrameWrapper>,
        public base::ISource<AVFrameWrapper>
    {
    private:
        SwrContext *_wrapped_obj = nullptr;

        /// @brief send_frame 方法被送入空指针，开启了冲洗模式。
        std::atomic_bool _flushed = false;

        AudioStreamInfoCollection _in_stream_infos;
        AudioFrameInfoCollection _out_frame_infos;
        std::shared_ptr<AVFrameWrapper> _silence_frame;

        /// @brief send_frame 方法被调用，向重采样器送入帧时就会记录那个帧的 pts。
        /// @note _in_pts_when_send_frame 相当于未来的时间戳。只要将重采样缓冲区中的采样点播放完，
        /// 重采样器输出端的时间戳就会真正变成 _in_pts_when_send_frame。也就是输入端的时间戳
        /// 和输出端的时间戳不仅时间基不一样，还存在时间差。
        int64_t _in_pts_when_send_frame = 0;

        int ReadFrameInFlushingMode(AVFrameWrapper &output_frame);
        int ReadFrameInNonFlushingMode(AVFrameWrapper &output_frame);

    public:
        /// @brief 使用 IAudioStreamInfoCollection 接口初始化重采样器。
        /// 因为重采样器要为输入输出的帧打上时间戳，所以要求 in_stream_infos 和 out_frame_infos
        /// 的时间基有效。
        /// @param in_stream_infos 设置重采样器输入流。
        /// @param out_frame_infos 设置重采样器输出流。
        SwrContextWrapper(IAudioStreamInfoCollection const &in_stream_infos,
                          IAudioFrameInfoCollection const &out_frame_infos);

        ~SwrContextWrapper();

        SwrContext *&WrappedObj() override
        {
            return _wrapped_obj;
        }

        SwrContext *WrappedObj() const override
        {
            return _wrapped_obj;
        }

        /// @brief 获取重采样器内部的延迟。
        /// 重采样器内部有一个高阶滤波器，高阶滤波器需要不同时刻的采样值来做差分，
        /// 所以内部有一个采样点队列用来储存不同时刻的采样值。这会导致输出产生延迟。
        /// 此外，如果转换完的采样点没有被取走，则会被储存在内部的输出缓冲区中，这
        /// 也会产生延迟。此函数用来获取所有的这些延迟。
        ///
        /// @param base 延迟的基。
        /// @note 不是时间基，base 是时间基的倒数。
        /// 详见 https://www.yuque.com/qiaodangyi/yrin4p/ica2heo5g3kyre9t
        /// @note 使用时间基的倒数是因为 ffmpeg 默认你会取一个 (0, 1] 内的，并且很接近 0 的数作为时间基，因为
        /// 这样精度才高。整型无法表示这种数，所以取了倒数。不用 double 是因为 double 在表示很小的数，例如
        /// 1 / 90000 时精度不能满足要求。这种时候用整型可以没有误差。time_base = 1 / 90000，则 base = 90000。
        ///
        /// @return 返回值 = 延迟时间 * base
        int64_t GetDelay(int64_t base);

        /// @brief 根据构造时传入的输出采样率来计算延迟为多少个输出采样点
        /// @return 延迟为多少个输出采样点
        int GetDelayAsSampleCount();

        /// @brief 估算当向此重采样器输入 in_nb_samples 数量的采样点时，输出将会是多少采样点。
        /// 此估计值会比实际值偏大，因此可以用来分配一个足够大的缓冲区
        /// @note 相关原理可以查看 https://www.yuque.com/qiaodangyi/yrin4p/dgyse5lne123ovav
        /// 本来我是根据这个原理手动写一个函数的，但是现在根据 swr_get_out_samples 了。
        /// 因为 swr_get_out_samples 计算出来的值比我手写的那个还大。不知道还有什么机制没了解的。
        ///
        /// @param in_nb_samples 将要向重采样器输入多少采样点
        /// @return 将会输出多少采样点
        int AvaliableSampleCount(int in_nb_samples);

        /// @brief 检查重采样器输出缓冲区中的数据是否足够填满 output_frame
        /// @param output_frame
        /// @return 足够填满则返回 true，不够则返回 false。
        bool CanFillOutputFrame(AVFrameWrapper const &output_frame)
        {
            // GetDelayAsSampleCount 是公共方法，已经加锁了，所以这里不用加锁。
            return GetDelayAsSampleCount() > output_frame.SampleCount() + 50;
        }

        /// @brief 是否已启动冲洗模式。
        /// @return 在调用 send_frame 方法时传入空指针就会设置此标志位，
        /// 下一次调用 read_frame 方法将会以冲洗的方式读出重采样器内的数据。
        bool Flushed()
        {
            return _flushed;
        }

        /// @brief 向重采样器送入帧。
        /// @param input_frame 要输入重采样器的帧。可以传入空指针。传入空指针会启动冲洗模式。
        void SendData(AVFrameWrapper &input_frame) override;

        void Flush() override;

        /// @brief 将重采样器中的数据取出来
        /// @param output_frame 用于接收重采样输出的数据。接收成功后 output_frame
        /// 的时间基会被设置为初始化本重采样器时的时间基，同时将为 output_frame 打上时间戳。
        ///
        /// @return 读取成功返回 0，失败返回错误代码。
        /// @note 非冲洗模式：
        /// 	@li 如果成功读取到完整的帧，则返回 0
        /// 	@li 如果重采样器内部的数据不足以填充完整的帧，返回
        /// 		ErrorCode::output_is_temporarily_unavailable，
        /// 		表示需要更多输入数据。
        /// @note 冲洗模式：
        /// 	@li 如果重采样器内有数据，则有多少拿多少，直到填充满输出帧或者拿完了。如果拿完后仍然不够填满输出帧，
        /// 		则会将输出帧后面没被填充的空隙填充为静音。此时也是返回 0
        /// 	@li 如果重采样器内部没有数据，则返回 ErrorCode::eof，表示到达文件尾。
        int ReadData(AVFrameWrapper &output_frame);
    };
} // namespace video
