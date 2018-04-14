

//
#pragma once


//
#include "BitEvolver/Includes.h"


//
#include <memory>
#include <vector>
#include <mutex>
#include <functional>


//
namespace BitEvolver
{
	//
	class Population
	{
		//
		public:
			
			//
			Population();
			
			//
			void Reset();
			
			//
			void ClearPopulation();
			void InitRandomPopulation(int _population_size, int _bit_length);
			void RandomizePopulation(int _bit_length);
			
			//
			void PopulationChanged();
			
			//
			std::vector<std::shared_ptr<class Chromosome>> GetChromosomes();
			void GetChromosomes(std::shared_ptr<std::vector<std::shared_ptr<class Chromosome>>> _chromosomes);
			std::shared_ptr<class Chromosome> GetChampion();
			
			//
			double GetAverageFitness();
			double GetAverageFitness(std::vector<std::shared_ptr<class Chromosome>> _chromosomes);
			
			//
			void SetCrossoverPoint(double p);
			double GetCrossoverPoint();
			void SetCrossoverType(Enums::CrossoverType t);
			Enums::CrossoverType GetCrossoverType();
			//
			void SetMutationRate(double r);
			double GetMutationRate();
			
			//
			void EvaluateFitness(std::function<double(std::shared_ptr<Chromosome>)> evaluation_callback);
			
			//
			void Evolve();
			int GetEvolutionNumber();
			
			//
			void PrintPopulation();
			void PrintPopulation(std::vector<std::shared_ptr<class Chromosome>> _chromosomes);
			
			//	Constants
			const static int DEFAULT_POPULATION_SIZE = 100;
			const static int DEFAULT_MUTATION_RATE = 0.01;
			//
			const static Enums::CrossoverType DEFAULT_CROSSOVER_TYPE = Enums::CrossoverType::Sexual;
			const static int DEFAULT_CROSSOVER_POINT = 0.7;
			
		//
		private:
			
			//
			std::shared_ptr<class Random> random;
			std::shared_ptr<class Breeder> breeder;
			
			//
			std::vector<std::shared_ptr<class Chromosome>> chromosomes;
			int population_size;
			bool population_needs_sorting;
			Enums::CrossoverType crossover_type;
			double crossover_point;
			double mutation_rate;
			int evolution_number;
			
			//
			std::shared_ptr<class RouletteWheel> roulette_wheel;
			
			//
			std::recursive_mutex
				population_modification_mutex,
				breed_mutex,
				evaluate_fitness_mutex
				;
			
			//
			void InitRandomGenerator();
			void InitRouletteWheel();
			void InitBreeder();
			
			//
			void EnsureSortedPopulation();
			
			//
			void BreedNewPopulation(std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> population_new, int size);
			void BreedNewPopulation_Thread(std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> population_new, int size);
			
			//
			void EvaluateFitness_Thread(
				std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes,
				std::function<double(std::shared_ptr<Chromosome>)> evaluation_callback
			);
			
			//
			std::shared_ptr<Chromosome> BreedChild();
			
			//
			int GetThreadCountSuggestion();
	};
};



