#pragma once
#include<ffmpeg-wrapper/base_include.h>
#include<base/Wrapper.h>

namespace video
{
	class AVFilterContextWrapper :public base::Wrapper<AVFilterContext>
	{
		AVFilterContext *_wrapped_obj = nullptr;

	public:
		AVFilterContextWrapper();
		AVFilterContextWrapper(AVFilterContext *filter_ctx);
		AVFilterContextWrapper(AVFilterContextWrapper const &other);
		AVFilterContextWrapper &operator=(AVFilterContextWrapper const &other);

		AVFilterContext *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		AVFilterContext *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		/// <summary>
		///		将下一个过滤器的输入端连接到本过滤器输出端。本过滤器的输出会作为下一个过滤器的输入。
		/// </summary>
		/// <param name="next_filter"></param>
		void link(AVFilterContextWrapper &next_filter);

		/// <summary>
		///		将 next_filter 的输入端连接到本对象的输出端。本对象的输出会作为 next_filter 的输入。
		/// </summary>
		/// <param name="next_filter">要连接到本对象输出端的过滤器。</param>
		/// <returns>
		///		返回 next_filter，所以 next_filter 右侧还可以继续用 &lt;&lt; (左移位) 运算符，
		///		跟着一个要连接到 next_filter 输出端的过滤器。
		/// </returns>
		AVFilterContextWrapper &operator<<(AVFilterContextWrapper &next_filter);
	};
}