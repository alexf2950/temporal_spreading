#include <random>
#include <iostream>

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0,1.0);

int SampleProbability(const double& probability)
{
	double number = distribution(generator);
	if(number<probability){
		// Its a hit
		return 1;
	}
	return 0;
}