#include "AVCalculate.h"

base::Fraction video::ToFraction(AVRational const &r)
{
    return base::Fraction{r.num, r.den};
}
