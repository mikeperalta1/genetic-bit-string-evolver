

//
#pragma once


//
#include "BitEvolver/Includes.h"


//
#include <memory>
#include <vector>
#include <mutex>


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
			void SetMutationRate(double r);
			
			//
			void Evolve();
			int GetEvolutionNumber();
			
			//
			void PrintPopulation();
			void PrintPopulation(std::vector<std::shared_ptr<class Chromosome>> _chromosomes);
			
		//
		private:
			
			//
			std::shared_ptr<class Random> random;
			std::shared_ptr<class Breeder> breeder;
			
			//
			std::vector<std::shared_ptr<class Chromosome>> chromosomes;
			int population_size;
			bool population_needs_sorting;
			double mutation_rate;
			int evolution_number;
			
			//
			std::mutex breed_mutex;
			
			//
			void InitRandomGenerator();
			void InitBreeder();
			
			//
			void EnsureSortedPopulation();
			
			//
			void BreedNewPopulation(std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> population_new, int size);
			void BreedNewPopulation_Thread(std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> population_new, int size);
			
			//
			std::shared_ptr<Chromosome> BreedChild();
			std::shared_ptr<Chromosome> PickChromosomeForBreeding();
			
	};
};



