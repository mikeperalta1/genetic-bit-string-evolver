

//
#pragma once


//
namespace BitEvolver
{
	//
	class Breeder
	{
		//
		public:
			
			//
			Breeder(std::shared_ptr<class Random> _random);
			
			//
			std::shared_ptr<class Chromosome> Breed(
				std::shared_ptr<class Chromosome> mama,
				std::shared_ptr<class Chromosome> papa,
				double crossover_rate,
				double mutation_rate
			);
			
			//
			void Mutate(std::shared_ptr<class Chromosome> chromosome, double mutation_rate);
			
		//
		private:
			
			//
			std::shared_ptr<class Random> random;
			
			//
			int PickRandomCrossoverPoint(std::shared_ptr<class Chromosome> chromosome, double crossover_rate);
			void ApplyCrossover(std::shared_ptr<class Chromosome> kiddo, std::shared_ptr<class Chromosome> parent, double crossover_rate);
	};
};

