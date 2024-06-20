#include "RandomSignalSource.h"
#include <random>

double video::RandomSignalSource::sample()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distr(-1.0, 1.0); // 定义一个均匀分布的浮点数分布
	double random_number = distr(gen); // 生成随机数
	return random_number;
}
