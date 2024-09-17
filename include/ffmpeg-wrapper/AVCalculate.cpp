#include "AVCalculate.h"

base::Fraction video::ToFraction(AVRational const &r)
{
    return base::Fraction{r.num, r.den};
}

int64_t video::ConvertTimeStamp(int64_t in_time_stamp,
                                AVRational const &in_time_base,
                                AVRational const &out_time_base)
{
    // 时间戳转换 https://www.yuque.com/qiaodangyi/yrin4p/vn2y64n75vfgi3o3
    int64_t out_pts = av_rescale_q(in_time_stamp, in_time_base, out_time_base);
    return out_pts;
}
