#include "ErrorCode.h"

std::string base::ToString(video::ErrorCode error_code)
{
	char err_buffer[1024];
	int result = av_strerror((int)error_code,
							 err_buffer,
							 sizeof(err_buffer));

	if (result == 0)
	{
		return std::string{err_buffer};
	}

	return "找不到该错误代码";
}
