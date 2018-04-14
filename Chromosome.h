

//
#pragma once


//
#include "BitEvolver/Includes.h"


//
#include <memory>
#include <string>
#include <sstream>
#include <vector>


//
namespace BitEvolver
{
	//
	class Chromosome
	{
		//
		public:
			
			//
			Chromosome(std::shared_ptr<class Random> _random, int _bits);
			
			//
			void Reset();
			void Randomize();
			
			//
			void SetBitCount(int count);
			int GetBitCount();
			
			//
			void FlipBit(int index);
			
			//
			bool GetBit(int index);
			void SetBit(int index, bool b);
			
			//
			void ResetFitness();
			void SetFitness(double d);
			void AdjustFitness(double d);
			double GetFitness();
			
			//
			std::string ToString();
			
			//
			const Chromosome& operator=(const Chromosome& other);
			
		//
		private:
			
			//	Random number generator
			std::shared_ptr<class Random> random;
			
			//
			std::vector<bool> bits;
			int bits_count_desired;
			
			//	Fitness
			double fitness;
	};
};

