#pragma once
#include "ffmpeg-wrapper/ffmpeg.h"
#include <base/Wrapper.h>


namespace video
{
	/// @brief 滤镜上下文
	class AVFilterContextWrapper :
		public base::Wrapper<AVFilterContext>
	{
	private:
		AVFilterContext *_wrapped_obj = nullptr;

	public:
		AVFilterContextWrapper() = default;
		AVFilterContextWrapper(AVFilterContext *o);
		AVFilterContextWrapper(AVFilterContextWrapper const &o);
		AVFilterContextWrapper &operator=(AVFilterContextWrapper const &o);

		AVFilterContext *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		/// @brief 将下一个过滤器的输入端连接到本过滤器输出端。本过滤器的输出会作为下一个过滤器的输入。
		/// @param next_filter
		void Link(AVFilterContextWrapper &next_filter);

		/// @brief 将 next_filter 的输入端连接到本对象的输出端。本对象的输出会作为 next_filter 的输入。
		/// @param next_filter 要连接到本对象输出端的过滤器。
		/// @return 返回 next_filter，所以 next_filter 右侧还可以继续用 << 运算符，
		/// 跟着一个要连接到 next_filter 输出端的过滤器。
		AVFilterContextWrapper &operator<<(AVFilterContextWrapper &next_filter);
	};
} // namespace video
