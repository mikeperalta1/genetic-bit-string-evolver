


//
#include "BitEvolver/Random.h"
#include "BitEvolver/RouletteWheel.h"
#include "BitEvolver/Chromosome.h"


//
#include <vector>
#include <mutex>
#include <algorithm>
#include <memory>
#include <iostream>


//
namespace BitEvolver
{
	//
	using std::cout;
	using std::endl;
	
	//
	RouletteWheel::RouletteWheel()
	{
		//
		this->Instantiate();
		
		//
		this->Reset();
	}
	
	//
	void RouletteWheel::Reset()
	{
		//
		this->ClearChromosomes();
	}
	
	//
	void RouletteWheel::ClearChromosomes()
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->chromosomes_mutex);
		
		//
		this->chromosomes.clear();
		this->ChromosomesChanged();
	}
	
	//
	void RouletteWheel::SetChromosomes(std::vector<std::shared_ptr<Chromosome>> _chromosomes)
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->chromosomes_mutex);
		
		//
		this->ClearChromosomes();
		
		//
		this->chromosomes = _chromosomes;
		
		//
		this->ChromosomesChanged();
	}
	
	//
	void RouletteWheel::AddChromosome(std::shared_ptr<Chromosome> _chromosome)
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->chromosomes_mutex);
		
		//
		this->chromosomes.push_back(_chromosome);
		this->ChromosomesChanged();
	}
	
	//
	void RouletteWheel::AddChromosomes(std::vector<std::shared_ptr<Chromosome>> _chromosomes)
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->chromosomes_mutex);
		
		//
		for ( std::shared_ptr<Chromosome> _chromosome  : _chromosomes ) {
			
			//
			this->chromosomes.push_back(_chromosome);
		}
		
		//
		this->ChromosomesChanged();
	}
	
	//
	std::shared_ptr<Chromosome> RouletteWheel::Spin()
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->chromosomes_mutex);
		double
			range_min, range_max,
			spin
			;
		size_t i;
		
		//
		this->PopulateSlots();
		
		//
		if ( !this->wheel_slots.size() ) {
			return nullptr;
		}
		
		//	Spin a random number in our range
		range_min = this->wheel_slots[0].first;
		range_max = this->wheel_slots[this->wheel_slots.size()-1].first;
		spin = this->random->GetDouble(range_min, range_max);
		
		//	Find the corresponding chromosome
		for ( i=0; i<this->wheel_slots.size(); i++ ) {
			if ( this->wheel_slots[i].first >= spin ) {
				return this->wheel_slots[i].second;
			}
		}
		
		//	By default, return the first one I guess
		return this->wheel_slots[0].second;
	}
	
	//
	void RouletteWheel::Instantiate()
	{
		//
		this->random = std::shared_ptr<Random>(
			new Random()
		);
	}
	
	//
	std::vector<std::pair<double, std::shared_ptr<Chromosome>>> RouletteWheel::GetNormalizedChromosomeFitness()
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->chromosomes_mutex);
		std::vector< std::pair< double, std::shared_ptr<Chromosome> > > pairs;
		std::pair< double, std::shared_ptr<Chromosome> > pair;
		double
			fitness, fitness_low, fitness_high
			;
		
		//
		if ( !this->chromosomes.size() ) {
			return pairs;
		}
		
		//	Locate lowest and highest fitness
		fitness_low = fitness_high = this->chromosomes[0]->GetFitness();
		for ( std::shared_ptr<Chromosome> chromosome : this->chromosomes ) {
			
			//
			fitness = chromosome->GetFitness();
			if ( fitness > fitness_high ) {
				fitness_high = fitness;
			}
			if ( fitness < fitness_low ) {
				fitness_low = fitness;
			}
		}
		
		//	Generate normalized pairs
		for ( std::shared_ptr<Chromosome> chromosome : this->chromosomes ) {
			
			//
			pair.first = chromosome->GetFitness() - fitness_low;
			pair.second = chromosome;
			
			//
			pairs.push_back(pair);
			
			//
			//cout << "[" << pair.first << "]" << chromosome->ToString() << endl;
		}
		
		return pairs;
	}
	
	//
	void RouletteWheel::SortChromosomes()
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->chromosomes_mutex);
		
		//
		if ( !this->chromosomes_need_sorting ) {
			return;
		}
		
		//
		std::sort(
			this->chromosomes.begin(),
			this->chromosomes.end(),
			[]( const std::shared_ptr<Chromosome>& left, const std::shared_ptr<Chromosome>& right ) -> bool
			{
				//
				if ( left->GetFitness() > right->GetFitness() ) {
					return true;
				}
				
				return false;
			}
		);
		
		//
		this->chromosomes_need_sorting = false;
	}
	
	//
	void RouletteWheel::PopulateSlots()
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->chromosomes_mutex);
		std::vector<std::pair<double, std::shared_ptr<Chromosome>>> chromosomes_normalized_fitness;
		std::pair<double,std::shared_ptr<Chromosome>> wheel_slot;
		double slot_begin_value;
		
		//
		if ( !this->slots_need_population ) {
			return;
		}
		
		//
		this->SortChromosomes();
		
		//
		this->wheel_slots.clear();
		
		//
		slot_begin_value = 0;
		chromosomes_normalized_fitness = this->GetNormalizedChromosomeFitness();
		for ( std::pair<double, std::shared_ptr<Chromosome>> pair: chromosomes_normalized_fitness ) {
			
			//
			wheel_slot.first = pair.first + slot_begin_value;
			wheel_slot.second = pair.second;
			
			//
			this->wheel_slots.push_back(wheel_slot);
			
			//
			slot_begin_value += pair.first;
		}
		
		//
		this->slots_need_population = false;
	}
	
	//
	void RouletteWheel::ChromosomesChanged()
	{
		//
		this->chromosomes_need_sorting = true;
		this->slots_need_population = true;
	}
};



