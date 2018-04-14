

//
#include "BitEvolver/Includes.h"
#include "BitEvolver/Random.h"
#include "BitEvolver/Population.h"
#include "BitEvolver/Breeder.h"
#include "BitEvolver/Chromosome.h"


//
#include <memory>
#include <vector>
#include <mutex>
#include <iostream>
#include <algorithm>
#include <thread>


//
namespace BitEvolver
{
	//
	using std::cout;
	using std::endl;
	
	//
	Population::Population()
	{
		//
		this->InitRandomGenerator();
		this->InitBreeder();
		
		//
		this->Reset();
	}
	
	//
	void Population::Reset()
	{
		//
		this->population_size = BIT_EVOLVER_POPULATION_DEFAULT_POPULATION_SIZE;
		this->SetMutationRate(BIT_EVOLVER_POPULATION_DEFAULT_MUTATE_RATE);
		this->evolution_number = 0;
		
		//
		this->RandomizePopulation(this->population_size);
	}
	
	//
	void Population::ClearPopulation()
	{
		//
		this->chromosomes.clear();
		this->evolution_number = 0;
	}
	
	//
	void Population::InitRandomPopulation(int _population_size, int _bit_length)
	{
		//
		this->population_size = _population_size;
		
		//
		this->RandomizePopulation(_bit_length);
	}
	
	//
	void Population::RandomizePopulation(int _bit_length)
	{
		//
		std::shared_ptr<Chromosome> chromosome;
		int i;
		
		//
		this->ClearPopulation();
		for ( i=0; i<this->population_size; i++ ) {
			
			//
			chromosome = std::shared_ptr<Chromosome>(
				new Chromosome( this->random, _bit_length )
			);
			this->chromosomes.push_back(chromosome);
		}
	}
	
	//
	void Population::PopulationChanged()
	{
		//
		this->population_needs_sorting = true;
	}
	
	//
	std::vector<std::shared_ptr<Chromosome>> Population::GetChromosomes()
	{
		return this->chromosomes;
	}
	
	//
	void Population::GetChromosomes(std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes)
	{
		//
		_chromosomes->clear();
		for ( std::shared_ptr<Chromosome> chromosome : this->chromosomes) {
			_chromosomes->push_back(chromosome);
		}
	}
	
	//
	std::shared_ptr<Chromosome> Population::GetChampion()
	{
		//
		this->EnsureSortedPopulation();
		
		//
		if ( this->chromosomes.size() > 0 ) {
			return this->chromosomes[0];
		}
		
		return nullptr;
	}
	
	//
	double Population::GetAverageFitness()
	{
		return this->GetAverageFitness(this->chromosomes);
	}
	
	//
	double Population::GetAverageFitness(std::vector<std::shared_ptr<Chromosome>> _chromosomes)
	{
		//
		double fitness_sum;
		double fitness_average;
		
		//
		fitness_sum = 0;
		for ( std::shared_ptr<Chromosome> chromosome : _chromosomes ) {
			fitness_sum += chromosome->GetFitness();
		}
		
		//
		fitness_average = 0;
		if ( _chromosomes.size() > 0 ) {
			fitness_average = fitness_sum / _chromosomes.size();
		}
		
		return fitness_average;
	}
	
	//
	void Population::SetMutationRate(double r)
	{
		//
		this->mutation_rate = r;
	}
	
	//
	void Population::Evolve()
	{
		//
		std::shared_ptr<std::vector< std::shared_ptr<Chromosome> > > population_new;
		
		//
		if ( this->chromosomes.size() == 0 ) {
			return;
		}
		
		//
		this->EnsureSortedPopulation();
		
		//
		population_new = std::shared_ptr<
			std::vector<
				std::shared_ptr<Chromosome>
			>
		>(
			new std::vector<std::shared_ptr<Chromosome>>()
		);
		
		//	Start the new population off with our champion,
		//	so the best score always carries over (elitism = 1 unit)
		#warning "Elitism is only 1 right now"
		population_new->push_back(this->chromosomes[0]);
		
		//	Breed the new population
		this->BreedNewPopulation(population_new, (int)this->chromosomes.size());
		
		//	Replace old population with the new
		this->chromosomes = *population_new;
		this->evolution_number++;
		
		//
		this->PopulationChanged();
	}
	
	//
	int Population::GetEvolutionNumber()
	{
		return this->evolution_number;
	}
	
	//
	void Population::PrintPopulation()
	{
		//
		this->EnsureSortedPopulation();
		
		this->PrintPopulation(this->chromosomes);
	}
	
	//
	void Population::PrintPopulation(std::vector<std::shared_ptr<Chromosome>> _chromosomes)
	{
		//
		for ( std::shared_ptr<Chromosome> chromosome : chromosomes ) {
			cout << chromosome->ToString() << endl;
		}
		cout << "Average Fitness --> " << this->GetAverageFitness(_chromosomes) << endl;
	}
	
	//
	void Population::InitRandomGenerator()
	{
		//
		this->random = std::shared_ptr<Random>(
			new Random()
		);
	}
	
	//
	void Population::InitBreeder()
	{
		//
		if ( !this->random ) {
			throw std::runtime_error("Population::InitBreeder() - Should come after InitRandomGenerator()");
		}
		
		//
		this->breeder = std::shared_ptr<Breeder>(
			new Breeder( this->random )
		);
	}
	
	//
	void Population::EnsureSortedPopulation()
	{
		//
		if ( !this->population_needs_sorting ) {
			return;
		}
		
		//	Yay std::sort
		std::sort(
			this->chromosomes.begin(),
			this->chromosomes.end(),
			[]( std::shared_ptr<Chromosome>& left, std::shared_ptr<Chromosome>& right ) -> bool
			{
				//
				if ( left->GetFitness() > right->GetFitness() ) {
					return true;
				}
				
				return false;
			}
		);
		
		//
		this->population_needs_sorting = false;
	}
	
	//
	void Population::BreedNewPopulation(std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> population_new, int size)
	{
		//
		std::vector<std::shared_ptr<std::thread>> threads;
		std::shared_ptr<std::thread> thread;
		int
			thread_count,
			i
			;
		
		//
		thread_count = this->GetThreadCountSuggestion();
		
		//
		for ( i=0; i<thread_count; i++) {
			thread = std::shared_ptr<std::thread>(
				new std::thread(&Population::BreedNewPopulation_Thread, this, population_new, size)
			);
			threads.push_back(thread);
		}
		
		//
		for ( i=0; i<(int)threads.size(); i++) {
			threads[i]->join();
		}
	}
	
	//
	void Population::BreedNewPopulation_Thread(std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> population_new, int size)
	{
		//
		std::shared_ptr<Chromosome> kiddo;
		
		//
		while ( (int)population_new->size() < size )
		{
			//
			kiddo = this->BreedChild();
			
			//	Mutexed
			this->breed_mutex.lock();
			if ( (int)population_new->size() < size ) {
				population_new->push_back(kiddo);
			}
			this->breed_mutex.unlock();
		}
	}
	
	//
	std::shared_ptr<Chromosome> Population::BreedChild()
	{
		//
		std::shared_ptr<Chromosome>
			mama, papa, kiddo
			;
		
		//	Pick two parents
		mama = this->PickChromosomeForBreeding();
		papa = this->PickChromosomeForBreeding();
		
		//
		kiddo = this->breeder->Breed(mama, papa, BIT_EVOLVER_POPULATION_DEFAULT_CROSSOVER, this->mutation_rate);
		
		return kiddo;
	}
	
	/**
		(1) We want to pick the best chromosomes to repopulate
		(2) [not in lecture] We want to add a bit of randomness
			to the selection process, such that the worst chromosomes
			can still possibly be picked, and the best can still
			possibly be not-picked; Their fitness only increases
			the probability they're picked.
	*/
	std::shared_ptr<Chromosome> Population::PickChromosomeForBreeding()
	{
		//
		double normal;
		int chromosome_index;
		
		//
		this->EnsureSortedPopulation();
		
		/**
			Grab normal with 0 at the mean, and the standard deviation equal
			to 1/2 of the population size. Then make that an absolute value.
			This will make top/best chromosomes more likely to be picked,
				with the far/low end being much less likely
		*/
		#warning "Need to upgrade this to Roulette Wheel"
		//	Repeat as needed, since the normal generator might actually
		//	give us an out-of-bounds result sometimes
		while ( true )
		{
			//
			normal = this->random->GetNormal(0, 0.5);
			chromosome_index = abs( normal * this->chromosomes.size()
			);
			if ( chromosome_index >= 0 && chromosome_index < (int)this->chromosomes.size() ) {
				break;
			}
		}
		
		//
		return this->chromosomes[chromosome_index];
	}
	
	//
	int Population::GetThreadCountSuggestion()
	{
		//
		int thread_count;
		
		//
		thread_count = std::thread::hardware_concurrency();
		
		return thread_count;
	}
};









