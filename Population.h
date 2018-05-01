

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
			void SetCrossoverType(Enums::CrossoverType t);
			Enums::CrossoverType GetCrossoverType();
			void SetCrossoverOrder(Enums::CrossoverOrder o);
			Enums::CrossoverOrder GetCrossoverOrder();
			void SetCrossoverBounds(Enums::CrossoverBounds b);
			Enums::CrossoverBounds GetCrossoverBounds();
			void SetCrossoverPoint(double p);
			double GetCrossoverPoint();
			void SetCrossoverPointStandardDeviation(double std);
			double GetCrossoverPointStandardDeviation();
			
			//
			void SetMutationRate(double r);
			double GetMutationRate();
			//
			void SetElitismType(Enums::ElitismType t);
			Enums::ElitismType GetElitismType();
			void SetElitismRate(double r);
			double GetElitismRate();
			void SetElitismCount(int c);
			int GetElitismCount();
			
			//
			void EvaluateFitness(std::function<double(std::shared_ptr<Chromosome>)> evaluation_callback);
			void EvaluateError(std::function<double(std::shared_ptr<Chromosome>)> evaluation_callback);
			
			//
			void Evolve();
			int GetEvolutionNumber();
			
			//
			void PrintPopulation();
			void PrintPopulation(std::vector<std::shared_ptr<class Chromosome>> _chromosomes);
			
			//	Constants
			const static int					DEFAULT_POPULATION_SIZE =		100;
			const static Enums::ElitismType		DEFAULT_ELITISM_TYPE =			Enums::ElitismType::Rate;
			constexpr static double				DEFAULT_ELITISM_RATE =			0.01;
			const static int					DEFAULT_ELITISM_COUNT =			1;
			constexpr static double				DEFAULT_MUTATION_RATE =			0.01;
			//
			const static Enums::CrossoverType	DEFAULT_CROSSOVER_TYPE =		Enums::CrossoverType::Sexual;
			const static Enums::CrossoverOrder	DEFAULT_CROSSOVER_ORDER = 		Enums::CrossoverOrder::MamaPapa;
			const static Enums::CrossoverBounds	DEFAULT_CROSSOVER_BOUNDS =		Enums::CrossoverBounds::Wrap;
			constexpr static double				DEFAULT_CROSSOVER_POINT =		0.7;
			constexpr static double				DEFAULT_CROSSOVER_POINT_STD =	0.25;
			
		//
		private:
			
			//
			std::shared_ptr<class Random> random;
			std::shared_ptr<class Breeder> breeder;
			
			//
			std::vector<std::shared_ptr<class Chromosome>> chromosomes;
			int population_size;
			bool population_needs_sorting;
			int evolution_number;
			
			//
			Enums::CrossoverType crossover_type;
			Enums::CrossoverOrder crossover_order;
			Enums::CrossoverBounds crossover_bounds;
			double crossover_point;
			double crossover_point_std;
			double mutation_rate;
			Enums::ElitismType elitism_type;
			double elitism_rate;
			int elitism_count;
			
			//
			std::shared_ptr<class RouletteWheel> roulette_wheel;
			
			//
			std::recursive_mutex
				population_modification_mutex,
				breed_mutex,
				evaluate_fitness_mutex,
				copy_chromosomes_mutex
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
			int DetermineEliteCount();
			void SeedPopulationWithElites(std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> population_new);
			
			//
			void EvaluateFitness_Thread(
				std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes,
				std::function<double(std::shared_ptr<Chromosome>)> evaluation_callback
			);
			void EvaluateError_Thread(
				std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes,
				std::function<double(std::shared_ptr<Chromosome>)> evaluation_callback
			);
			
			//
			void CopyChromosomes(
				std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes_source,
				std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes_destination
			);
			void CopyChromosomes_Thread(
				std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes_source,
				std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes_destination
			);
			
			//
			std::shared_ptr<Chromosome> BreedChild();
			
			//
			int GetThreadCountSuggestion();
	};
};



