


//
#pragma once


//
#include <vector>
#include <mutex>
#include <memory>


//
namespace BitEvolver
{
	//
	class RouletteWheel
	{
		//
		public:
			
			//
			RouletteWheel();
			
			//
			void Reset();
			
			//
			void ClearChromosomes();
			void SetChromosomes(std::vector<std::shared_ptr<class Chromosome>> _chromosomes);
			void AddChromosome(std::shared_ptr<class Chromosome> _chromosome);
			void AddChromosomes(std::vector<std::shared_ptr<class Chromosome>> _chromosomes);
			
			//
			std::shared_ptr<Chromosome> Spin();
			
		//
		private:
			
			//
			std::shared_ptr<class Random> random;
			
			//
			std::vector<std::shared_ptr<class Chromosome>> chromosomes;
			std::recursive_mutex chromosomes_mutex;
			bool chromosomes_need_sorting;
			//
			std::vector<std::pair<double,std::shared_ptr<class Chromosome>>> wheel_slots;
			bool slots_need_population;
			
			
			//
			void Instantiate();
			
			//
			std::vector<std::pair<double, std::shared_ptr<class Chromosome>>> GetNormalizedChromosomeFitness();
			void SortChromosomes();
			void PopulateSlots();
			
			//
			void ChromosomesChanged();
			
	};
};



