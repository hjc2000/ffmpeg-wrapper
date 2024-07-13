#pragma once
#include <base/Wrapper.h>
#include <base/pipe/ISource.h>
#include <chrono>
#include <ffmpeg-wrapper/base_include.h>
#include <ffmpeg-wrapper/wrapper/AVIOContextWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include <ffmpeg-wrapper/wrapper/AVProgramWrapper.h>
#include <functional>
#include <memory>
#include <string>

namespace video
{
	class AVPacketWrapper;
	class AVStreamWrapper;
	class AVCodecContextWrapper;

	class InputFormat
		: public base::Wrapper<AVFormatContext>,
		  public base::ISource<AVPacketWrapper>
	{
	private:
		AVFormatContext *_wrapped_obj = nullptr;
		std::string _url;
		std::shared_ptr<AVIOContextWrapper> _io_context;

		/// @brief 通过读几个包来检测流信息。此操作不会导致读取进度向前推移
		/// @param options
		void FindStreamInfo(::AVDictionary **options = nullptr);

	public:
		InputFormat(std::string url);
		InputFormat(std::string url, AVInputFormat const *fmt, AVDictionary **options);
		InputFormat(std::shared_ptr<AVIOContextWrapper> io_context);
		InputFormat(std::shared_ptr<base::Stream> input_stream);
		~InputFormat();

		AVFormatContext *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		AVFormatContext *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		/// @brief 以官方格式打印流信息。
		void DumpFormat();

		/// @brief 找出最好的流
		/// @param type
		///
		/// @return 返回 AVStreamWrapper 对象。
		/// @note AVStreamWrapper 是个包装类，有 IsNull 方法，
		/// 可以判断是否为空。为空的话说明找不到此类型的最好的流。
		AVStreamWrapper FindBestStream(AVMediaType type);

		/// @brief 从封装中读取包。
		/// @param data 读取到的包会写入这里。
		/// @return 成功返回 0，失败返回错误代码
		int ReadData(AVPacketWrapper &data) override;

		/// @brief 获取本格式的播放时长
		/// @return
		std::chrono::seconds DurationInSeconds();

		/// @brief 流的数量
		/// @return
		int StreamCount();

		/// @brief 获取指定索引的流。流的索引号超出范围会抛出异常
		/// @param stream_index
		/// @return
		AVStreamWrapper GetStream(int stream_index);
	};
}