#pragma once
#include <base/container/List.h>
#include <base/pipe/IConsumer.h>
#include <base/pipe/IPipeSource.h>
#include <ffmpeg-wrapper/AVCalculate.h>
#include <ffmpeg-wrapper/AVCompare.h>
#include <ffmpeg-wrapper/AVToString.h>
#include <ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include <memory>

namespace video
{
	class InfinitePacketPipe
		: public base::IConsumer<AVPacketWrapper>,
		  public base::IPipeSource<AVPacketWrapper>
	{
	private:
		int64_t _last_dts = 0;
		int64_t _offset = 0;
		int64_t _last_packet_duration = 0;
		base::List<std::shared_ptr<base::IConsumer<AVPacketWrapper>>> _consumer_list;

		void UpdateLastPacketDuration(int64_t value);
		void UpdateLastDts(int64_t value);

#pragma region 校准起始时间戳
		/// @brief _first_dts 字段还没被设置，需要设置后才能进行
		/// 起始 dts 的校正。
		bool _first_dts_not_set = true;
		int64_t _first_dts = 0;

		/// @brief 校正 dts 起点
		/// @note 如果有一个文件的第一个包的 dts 不等于 0，就记录到 _first_dts 中，
		/// 然后每次调用本方法，都会根据 _first_dts 进行校正。
		/// @param packet
		void CorrectStartingPointOfDts(AVPacketWrapper &packet);
#pragma endregion

	public:
		base::IList<std::shared_ptr<base::IConsumer<AVPacketWrapper>>> &ConsumerList() override
		{
			return _consumer_list;
		}

		/// @brief 送入包
		/// @param packet
		/// @exception 送进来的包的时间基必须是 1/90000，否则会抛出异常。
		void SendData(AVPacketWrapper &packet) override;

		void Flush();
	};
}
