#pragma once
#include<ffmpeg-wrapper/base_include.h>
#include<jccpp/ToString.h>
#include<base/Wrapper.h>

namespace video
{
	/**
	 * @brief 字典操作的枚举类型
	 */
	enum class AVDictionaryFlag
	{
		/**
		 * @brief 无特殊操作
		*/
		None = 0,

		/**
		 * @brief 仅匹配完全一致的键（大小写敏感）。仅在 av_dict_get() 中相关。
		 */
		MatchCase = AV_DICT_MATCH_CASE,

		/**
		 * @brief 返回字典中第一个与搜索键部分匹配的条目，忽略找到的键字符串的后缀。仅在 av_dict_get() 中相关。
		 */
		IgnoreSuffix = AV_DICT_IGNORE_SUFFIX,

		/**
		 * @brief 接管已使用 av_malloc() 或其他内存分配函数分配的键的所有权。
		 */
		DontStrdupKey = AV_DICT_DONT_STRDUP_KEY,

		/**
		 * @brief 接管已使用 av_malloc() 或其他内存分配函数分配的值的所有权。
		 */
		DontStrdupVal = AV_DICT_DONT_STRDUP_VAL,

		/**
		 * @brief 不覆盖现有条目。
		 */
		DontOverwrite = AV_DICT_DONT_OVERWRITE,

		/**
		 * @brief 如果条目已存在，则附加到该条目。注意，不添加分隔符，字符串只是简单地连接在一起。
		 */
		Append = AV_DICT_APPEND,

		/**
		 * @brief 允许在字典中存储多个相同的键。
		 */
		Multikey = AV_DICT_MULTIKEY,
	};

	/**
	 * @brief AVDictionary 是类似于链表的存在，有可能是二叉树之类的。
	 *
	 * 本类的析构函数会自动释放 AVDictionary，这可能与 ffmpeg 的某些函数重复释放。
	 * 如果 ffmpeg 的对象含有 AVDictionary* 字段，并且在调试时发现会重复释放，
	 * 则在调用 ffmpeg 释放函数之前将那个对象的 AVDictionary* 字段设为 nullprt，
	 * 就可避免重复释放。
	*/
	class AVDictionaryWrapper :public base::Wrapper<AVDictionary>, public ICanToString
	{
		bool _do_not_free_dic = false;
		AVDictionary *_wrapped_obj = nullptr;

	public:
		AVDictionaryWrapper() {}

		AVDictionaryWrapper(AVDictionary *dic)
		{
			_wrapped_obj = dic;
		}

		~AVDictionaryWrapper()
		{
			FreeInnerDictionary();
		}

		AVDictionary *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		AVDictionary *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		/**
		 * @brief 不要释放内部包装的字典，这是为了避免与 ffmpeg 中的某些 free 函数重复释放导致崩溃。
		 * @param do_not 设置为 true 后，本对象的 av_dict_free 方法不会生效。
		*/
		void DoNotFreeInnerDictionary(bool do_not = true)
		{
			_do_not_free_dic = do_not;
		}

		/**
		 * @brief 释放内部包装的字典。调用 DoNotFreeInnerDictionary 并传入 true 后，本方法不会生效。
		*/
		void FreeInnerDictionary()
		{
			if (_do_not_free_dic)
			{
				return;
			}

			av_dict_free(&_wrapped_obj);
			_wrapped_obj = nullptr;
		}

		/**
		 * @brief 获取字典中记录的条数
		 *
		 * @return int
		 */
		int Count() const
		{
			return ::av_dict_count(_wrapped_obj);
		}

		/**
		 * @brief 用 key 获取值。如果不存在指定的键值对，返回空指针。
		 * @param key
		 * @param previous_entry
		 * @param flags
		 * @return 如果不存在指定的键值对，返回空指针。
		*/
		char const *GetValueByKey(char const *key, const AVDictionaryEntry *previous_entry = nullptr, AVDictionaryFlag flags = AVDictionaryFlag::None)
		{
			AVDictionaryEntry *entry = GetEntryByKey(key, previous_entry, flags);
			if (entry)
			{
				return entry->value;
			}

			return nullptr;
		}

		/**
		 * @brief 向字典中写一条记录。如果已经存在相同 key 的键值对，则旧的值会被覆盖。
		 * @param key
		 * @param value
		 * @param flags
		 * @return int
		 */
		int SetValueByKey(const char *key, const char *value, AVDictionaryFlag flags = AVDictionaryFlag::None)
		{
			return ::av_dict_set(&_wrapped_obj, key, value, (int)flags);
		}

		/**
		 * @brief 获取字典中的记录
		 * @param key
		 * @param previous_entry 指定上一条记录，如果指定，则从这里开始往后查找，
		 * 如果传入 nullprt 则从头开始查找
		 *
		 * @param flags
		 * @return 如果找到了，返回该节点的指针。如果没找到，返回空指针。
		 */
		AVDictionaryEntry *GetEntryByKey(const char *key, const AVDictionaryEntry *previous_entry = nullptr, AVDictionaryFlag flags = AVDictionaryFlag::None) const
		{
			return ::av_dict_get(_wrapped_obj, key, previous_entry, (int)flags);
		}

		#pragma region 迭代器
		class AVDictionaryIterator
		{
		public:
			AVDictionaryIterator(AVDictionary *dic, AVDictionaryEntry *current_entry)
			{
				_dic = dic;
				_current_entry = current_entry;
			}

			AVDictionaryIterator &operator++()
			{
				_current_entry = av_dict_get(_dic, "", _current_entry, (int)AVDictionaryFlag::IgnoreSuffix);
				return *this;
			}

			AVDictionaryEntry *operator*() const
			{
				return _current_entry;
			}

			bool operator!=(const AVDictionaryIterator &other) const
			{
				return _current_entry != other._current_entry;
			}

		private:
			AVDictionary *_dic;
			AVDictionaryEntry *_current_entry;
		};

		AVDictionaryIterator begin() const
		{
			return AVDictionaryIterator(_wrapped_obj, av_dict_get(_wrapped_obj, "", nullptr, AV_DICT_IGNORE_SUFFIX));
		}

		AVDictionaryIterator end() const
		{
			return AVDictionaryIterator(_wrapped_obj, nullptr);
		}
		#pragma endregion

		std::string ToString() override
		{
			std::stringstream sb;
			for (AVDictionaryEntry *entry : *this)
			{
				sb << std::format("{} = {}", entry->key, entry->value) << endl;
			}

			return sb.str();
		}
	};
}