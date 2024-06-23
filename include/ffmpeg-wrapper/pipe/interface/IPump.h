#pragma once
#include <base/task/CancellationToken.h>
#include <memory>

namespace video
{
	class IPump
	{
	public:
		/// @brief 本类作为中转站。将数据从源拉过来送给下级的消费者。
		/// @param cancel_pump
		virtual void Pump(std::shared_ptr<base::CancellationToken> cancel_pump) = 0;
	};
}
