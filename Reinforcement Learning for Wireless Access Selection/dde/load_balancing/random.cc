#include "stdafx.h"

#ifdef linux
#include <cstdlib>
#include "load_balancing/random.h"
#endif

#ifdef _WIN32
#include "Random.h"
#endif

//Initialization function, which sets the starting point for generating a series of pseudorandom integers 
void srand48(unsigned int seed)
{
	srand(seed);
}

//Return non-negative values uniformly distributed between [0.0, 1.0).
double drand48()
{
	return (double(rand())/(RAND_MAX + 1.0));
}
