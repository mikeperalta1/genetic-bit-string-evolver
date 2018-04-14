

//
#pragma once


//
#include <random>


//
namespace BitEvolver
{
	//
	class Random
	{
		//
		public:
			
			//
			Random();
			
			//
			int GetInt(int min, int max);
			double GetDouble(double min, double max);
			double GetNormal(double mean, double standard_deviation);
			
			//
			bool RollBool(double chance);
			
		//
		private:
			
			//
			std::mt19937_64 generator_mersenne_twister;
			
			//
			void InitializeGenerators();
			
	};
};
