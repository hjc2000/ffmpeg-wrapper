#include "SawtoothWaveSignalSource.h"

double video::SawtoothWaveSignalSource::sample(double t)
{
	t = truncate_time_into_a_period(t);
	return slope() * t;
}
