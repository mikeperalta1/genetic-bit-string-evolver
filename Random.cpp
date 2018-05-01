

//
#include "BitEvolver/Random.h"


//
#include <random>
#include <chrono>


//
namespace BitEvolver
{
	//
	Random::Random()
	{
		//
		this->InitializeGenerators();
	}
	
	//
	int Random::GetInt(int min, int max)
	{
		//
		std::uniform_int_distribution<int> distribution(min, max);
		
		//
		int the_int = distribution(this->generator_mersenne_twister);
		
		return the_int;
	}
	
	//
	double Random::GetDouble(double min, double max)
	{
		//
		std::uniform_real_distribution<double> distribution(min, max);
		
		//
		double the_double = distribution(this->generator_mersenne_twister);
		
		return the_double;
	}
	
	//
	double Random::GetNormal(double mean, double standard_deviation)
	{
		//
		std::normal_distribution<double> distribution(mean, standard_deviation);
		
		double d = distribution(this->generator_mersenne_twister);
		
		return d;
	}
	
	//
	bool Random::RollBool(double chance)
	{
		//
		double d = this->GetDouble(0, 1);
		if ( d <= chance ) {
			return true;
		}
		
		return false;
	}
	
	//
	void Random::InitializeGenerators()
	{
		//	Mostly taken from
		//	http://www.cplusplus.com/reference/random/mersenne_twister_engine/seed/
		typedef std::chrono::high_resolution_clock myclock;
		
		//
		myclock::time_point beginning = myclock::now();
		myclock::duration d = myclock::now() - beginning;
		unsigned seed = d.count();
		
		//	Seed our internal generator
		this->generator_mersenne_twister.seed(seed);
	}
};


