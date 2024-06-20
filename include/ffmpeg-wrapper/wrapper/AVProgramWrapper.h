#pragma once
#include<base/container/List.h>
#include<base/Wrapper.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/wrapper/AVDictionaryWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include<jccpp/ToString.h>

namespace video
{
	class InputFormat;

	/**
	 * @brief AVProgramWrapper 不拥有 AVProgram 对象，只是接收一个 AVProgram * 然后包装起来方便操作而已。
	 * AVProgramWrapper 析构的时候不会释放有关 AVProgram 的资源。这是由 AVFormatContext 的 avformat_free_context
	 * 函数负责释放的。
	*/
	class AVProgramWrapper :public base::Wrapper<AVProgram>, public ICanToString
	{
		AVProgram *_wrapped_obj = nullptr;
		shared_ptr<AVDictionaryWrapper> _dic;

	public:
		/**
		* @brief 构造一个空的 AVProgramWrapper 对象，没有包装任何的 AVProgram 对象。
		* 此时调用 AVProgramWrapper 的任何方法都会引发访问空指针的错误。
		*
		* 设置此构造函数只是为了能够将 AVProgramWrapper 变量的定义和初始化分开。通过此构造函数构造的
		* AVProgramWrapper 对象必须通过赋值运算符进行初始化。
		*/
		AVProgramWrapper()
		{

		}

		/**
		* @brief 包装一个 AVProgram 对象，但不拥有它，不会释放 AVProgram 对象内的资源。
		* AVProgram 对象的主人是 AVFormatContext，avformat_free_context 函数会释放它的资源。
		*
		* @param program
		*/
		AVProgramWrapper(AVProgram *program)
		{
			_wrapped_obj = program;
			_dic = unique_ptr<AVDictionaryWrapper> { new AVDictionaryWrapper { _wrapped_obj->metadata } };

			// 需要设置为不释放字典，否则会与 avformat_free_context 函数重复释放。
			_dic->DoNotFreeInnerDictionary();
		}

		AVProgramWrapper(AVProgramWrapper const &other)
		{
			_wrapped_obj = other._wrapped_obj;
			_dic = other._dic;
		}

		AVProgramWrapper &operator=(AVProgramWrapper const &other)
		{
			_wrapped_obj = other._wrapped_obj;
			_dic = other._dic;
			return *this;
		}

		AVProgram *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		AVProgram *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		int service_id()
		{
			return _wrapped_obj->id;
		}
		void set_service_id(int value)
		{
			_wrapped_obj->id = value;
		}

		void set_service_name(const char *service_name)
		{
			_dic->SetValueByKey("service_name", service_name);
			_wrapped_obj->metadata = *_dic;
		}

		void set_service_provider(const char *service_provider)
		{
			_dic->SetValueByKey("service_provider", service_provider);
			_wrapped_obj->metadata = *_dic;
		}

		/**
		 * @brief 本节目中有多少个流索引。
		 * 流索引用来在格式中找到流。
		 *
		 * @return
		*/
		int StreamIndexCount()
		{
			return _wrapped_obj->nb_stream_indexes;
		}

		/**
		 * @brief 获取指定位置的流索引。
		 * @param index 与 StreamIndexCount 属性对应。表示你想要第几个流索引。
		 * @return 返回指定位置的流索引。
		*/
		int GetStreamIndex(int index)
		{
			return _wrapped_obj->stream_index[index];
		}

		std::string ToString() override
		{
			return _dic->ToString();
		}

		operator bool()
		{
			return _wrapped_obj;
		}
	};
}