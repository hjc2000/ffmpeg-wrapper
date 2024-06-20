#pragma once
#include<chrono>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketSource.h>
#include<ffmpeg-wrapper/wrapper/AVIOContextWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVProgramWrapper.h>
#include<functional>
#include<base/Wrapper.h>
#include<string>

namespace video
{
	class AVPacketWrapper;
	class AVStreamWrapper;
	class AVCodecContextWrapper;

	class InputFormat :
		public base::Wrapper<AVFormatContext>,
		public IPacketSource
	{
	private:
		AVFormatContext *_wrapped_obj = nullptr;
		string _url;
		shared_ptr<AVIOContextWrapper> _io_context;

		/// <summary>
		///		通过读几个包来检测流信息。此操作不会导致读取进度向前推移
		/// </summary>
		/// <param name="options"></param>
		void FindStreamInfo(::AVDictionary **options = nullptr);

	public:
		InputFormat(std::string url);
		InputFormat(std::string url, AVInputFormat const *fmt, AVDictionary **options);
		InputFormat(shared_ptr<AVIOContextWrapper> io_context);
		InputFormat(shared_ptr<base::Stream> input_stream);
		~InputFormat();

		AVFormatContext *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		AVFormatContext *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		/// <summary>
		///		以官方格式打印流信息。
		/// </summary>
		void DumpFormat();

		/// <summary>
		///		找出最好的流
		/// </summary>
		/// <param name="type"></param>
		/// <returns>
		///		返回 AVStreamWrapper 对象。AVStreamWrapper 是个包装类，包装类重载了 bool 转换运算符，
		///		可以判断是否为空。
		/// </returns>
		AVStreamWrapper FindBestStream(AVMediaType type);

		/// <summary>
		///		从封装中读取包。
		/// </summary>
		/// <param name="packet">读取到的包会写入这里。</param>
		/// <returns>成功返回 0，失败返回错误代码</returns>
		int ReadPacket(AVPacketWrapper &packet) override;

		/// <summary>
		///		获取本格式的播放时长
		/// </summary>
		/// <returns></returns>
		std::chrono::seconds DurationInSeconds();

		/// <summary>
		///		流的数量
		/// </summary>
		/// <returns></returns>
		int StreamCount();

		/// <summary>
		///		获取指定索引的流。流的索引号超出范围会抛出异常
		/// </summary>
		/// <param name="stream_index"></param>
		/// <returns></returns>
		AVStreamWrapper GetStream(int stream_index);
	};
}