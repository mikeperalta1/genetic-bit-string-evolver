

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
				Enums::CrossoverType crossover_type,
				Enums::CrossoverOrder crossover_order,
				Enums::CrossoverBounds crossover_bounds,
				double crossover_point,
				double crossover_point_std,
				double mutation_rate
			);
			
			//
			void Mutate(std::shared_ptr<class Chromosome> chromosome, double mutation_rate);
			
		//
		private:
			
			//
			std::shared_ptr<class Random> random;
			
			//
			int PickRandomCrossoverPoint(
				std::shared_ptr<class Chromosome> chromosome,
				Enums::CrossoverBounds crossover_bounds,
				double crossover_point,
				double crossover_point_std
			);
			void ApplyCrossover(
				std::shared_ptr<class Chromosome> kiddo,
				std::shared_ptr<class Chromosome> parent,
				Enums::CrossoverType crossover_type,
				Enums::CrossoverBounds crossover_bounds,
				double crossover_point,
				double crossover_point_std
			);
	};
};

