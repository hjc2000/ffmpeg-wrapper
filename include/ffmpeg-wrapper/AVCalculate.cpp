#include<ffmpeg-wrapper/AVCalculate.h>

int64_t operator*(int64_t const int_num, AVRational const rational)
{
	return int_num * rational.num / rational.den;
}

int64_t operator*(AVRational const rational, int64_t const int_num)
{
	return int_num * rational;
}

AVRational operator*(AVRational r1, AVRational r2)
{
	return av_mul_q(r1, r2);
}

AVRational operator/(AVRational r1, AVRational r2)
{
	return av_div_q(r1, r2);
}

AVRational reciprocal(AVRational rational)
{
	AVRational ret{};
	ret.den = rational.num;
	ret.num = rational.den;
	return ret;
}

int64_t ConvertTimeStamp(int64_t in_time_stamp, AVRational in_time_base, AVRational out_time_base)
{
	// 时间戳转换 https://www.yuque.com/qiaodangyi/yrin4p/vn2y64n75vfgi3o3
	int64_t out_pts = av_rescale_q(in_time_stamp, in_time_base, out_time_base);
	return out_pts;
}
