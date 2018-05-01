

//
#include "BitEvolver/Includes.h"
#include "BitEvolver/Random.h"
#include "BitEvolver/Population.h"
#include "BitEvolver/Breeder.h"
#include "BitEvolver/RouletteWheel.h"
#include "BitEvolver/Chromosome.h"


//
#include <memory>
#include <vector>
#include <mutex>
#include <iostream>
#include <algorithm>
#include <thread>
#include <functional>
#include <string>
#include <sstream>


//
namespace BitEvolver
{
	//
	using std::string;
	using std::stringstream;
	using std::cout;
	using std::endl;
	
	//
	Population::Population()
	{
		//
		this->InitRandomGenerator();
		this->InitRouletteWheel();
		this->InitBreeder();
		
		//
		this->Reset();
	}
	
	//
	void Population::Reset()
	{
		//
		this->evolution_number = 0;
		this->population_size = Population::DEFAULT_POPULATION_SIZE;
		
		//
		this->SetElitismType(Population::DEFAULT_ELITISM_TYPE);
		this->SetElitismRate(Population::DEFAULT_ELITISM_RATE);
		this->SetElitismCount(Population::DEFAULT_ELITISM_COUNT);
		
		//
		this->SetCrossoverType(Population::DEFAULT_CROSSOVER_TYPE);
		this->SetCrossoverOrder(Population::DEFAULT_CROSSOVER_ORDER);
		this->SetCrossoverBounds(Population::DEFAULT_CROSSOVER_BOUNDS);
		this->SetCrossoverPoint(Population::DEFAULT_CROSSOVER_POINT);
		this->SetCrossoverPointStandardDeviation(Population::DEFAULT_CROSSOVER_POINT_STD);
		
		//
		this->SetMutationRate(Population::DEFAULT_MUTATION_RATE);
		
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
	void Population::SetCrossoverType(Enums::CrossoverType t)
	{
		//
		this->crossover_type = t;
	}
	
	//
	Enums::CrossoverType Population::GetCrossoverType()
	{
		//
		return this->crossover_type;
	}
	
	//
	void Population::SetCrossoverOrder(Enums::CrossoverOrder o)
	{
		//
		this->crossover_order = o;
	}
	
	//
	Enums::CrossoverOrder Population::GetCrossoverOrder()
	{
		//
		return this->crossover_order;
	}
	
	//
	void Population::SetCrossoverBounds(Enums::CrossoverBounds b)
	{
		//
		this->crossover_bounds = b;
	}
	
	//
	Enums::CrossoverBounds Population::GetCrossoverBounds()
	{
		//
		return this->crossover_bounds;
	}
	
	//
	void Population::SetCrossoverPoint(double p)
	{
		//
		this->crossover_point = p;
	}
	
	//
	double Population::GetCrossoverPoint()
	{
		//
		return this->crossover_point;
	}
	
	//
	void Population::SetCrossoverPointStandardDeviation(double std)
	{
		//
		this->crossover_point_std = std;
	}
	
	//
	double Population::GetCrossoverPointStandardDeviation()
	{
		//
		return this->crossover_point_std;
	}
	
	//
	void Population::SetMutationRate(double r)
	{
		//
		this->mutation_rate = r;
	}
	
	//
	double Population::GetMutationRate()
	{
		//
		return this->mutation_rate;
	}
	
	//
	void Population::SetElitismType(Enums::ElitismType t)
	{
		//
		this->elitism_type = t;
	}
	
	//
	Enums::ElitismType Population::GetElitismType()
	{
		//
		return this->elitism_type;
	}
	
	//
	void Population::SetElitismRate(double r)
	{
		//
		this->elitism_rate = r;
	}
	
	//
	double Population::GetElitismRate()
	{
		//
		return this->elitism_rate;
	}
	
	//
	void Population::SetElitismCount(int c)
	{
		//
		this->elitism_count = c;
	}
	
	//
	int Population::GetElitismCount()
	{
		//
		return this->elitism_count;
	}
	
	//
	void Population::EvaluateFitness(std::function<double(std::shared_ptr<Chromosome>)> evaluation_callback)
	{
		//
		std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes_copy;
		std::vector<std::shared_ptr<std::thread>> threads;
		std::shared_ptr<std::thread> thread;
		int
			threads_count,
			i
			;
		
		//	Make a new vector containing all current chromosomes
		this->population_modification_mutex.lock();
		_chromosomes_copy = std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>>(
			new std::vector<std::shared_ptr<Chromosome>>()
		);
		for ( i=0; i<(int)this->chromosomes.size(); i++ ) {
			_chromosomes_copy->push_back( this->chromosomes[i] );
		}
		this->population_modification_mutex.unlock();
		
		//	Spawn threads
		threads_count = this->GetThreadCountSuggestion();
		for ( i=0; i<threads_count; i++) {
			
			//
			thread = std::shared_ptr<std::thread>(
				new std::thread(&Population::EvaluateFitness_Thread, this, _chromosomes_copy, evaluation_callback)
			);
			threads.push_back(thread);
		}
		
		//	Wait for threads to finish
		for ( i=0; i<threads_count; i++ ) {
			threads[i]->join();
		}
	}
	
	//
	void Population::EvaluateError(std::function<double(std::shared_ptr<Chromosome>)> evaluation_callback)
	{
		//
		std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes_copy;
		std::vector<std::shared_ptr<std::thread>> threads;
		std::shared_ptr<std::thread> thread;
		int
			threads_count,
			i
			;
		
		//	Make a new vector containing all current chromosomes
		this->population_modification_mutex.lock();
		_chromosomes_copy = std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>>(
			new std::vector<std::shared_ptr<Chromosome>>()
		);
		for ( i=0; i<(int)this->chromosomes.size(); i++ ) {
			_chromosomes_copy->push_back( this->chromosomes[i] );
		}
		this->population_modification_mutex.unlock();
		
		//	Spawn threads
		threads_count = this->GetThreadCountSuggestion();
		for ( i=0; i<threads_count; i++) {
			
			//
			thread = std::shared_ptr<std::thread>(
				new std::thread(&Population::EvaluateError_Thread, this, _chromosomes_copy, evaluation_callback)
			);
			threads.push_back(thread);
		}
		
		//	Wait for threads to finish
		for ( i=0; i<threads_count; i++ ) {
			threads[i]->join();
		}
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
	void Population::InitRouletteWheel()
	{
		//
		this->roulette_wheel = std::shared_ptr<RouletteWheel>(
			new RouletteWheel()
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
		
		//	First, populate the roulette wheel
		this->roulette_wheel->SetChromosomes(this->chromosomes);
		
		//	Next, seed the population with elites
		this->SeedPopulationWithElites(population_new);
		
		//	Next, breed until we've reached our new size
		thread_count = this->GetThreadCountSuggestion();
		for ( i=0; i<thread_count; i++) {
			thread = std::shared_ptr<std::thread>(
				new std::thread(&Population::BreedNewPopulation_Thread, this, population_new, size)
			);
			threads.push_back(thread);
		}
		for ( i=0; i<(int)threads.size(); i++) {
			threads[i]->join();
		}
		
		//	Finally, reset the fitness of the new population
		for ( i=0; i<(int)population_new->size(); i++ ) {
			population_new->at(i)->ResetFitness();
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
	int Population::DetermineEliteCount()
	{
		//
		int count;
		
		//
		switch( this->elitism_type )
		{
			//
			default:
			case Enums::ElitismType::None:
				count = 0;
				break;
			
			//
			case Enums::ElitismType::Absolute:
				count = this->elitism_count;
				break;
			
			//
			case Enums::ElitismType::Rate:
				count = floor( this->chromosomes.size() * this->elitism_rate );
				break;
		}
		
		return count;
	}
	
	//
	void Population::SeedPopulationWithElites(std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> population_new)
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->population_modification_mutex);
		std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> elites;
		std::vector<std::shared_ptr<std::thread>> threads;
		std::shared_ptr<std::thread> thread;
		int
			elites_count,
			i
			;
		
		//	Determine how many elites to copy
		elites_count = this->DetermineEliteCount();
		elites = std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>>(
			new std::vector<std::shared_ptr<Chromosome>>()
		);
		
		//	First, copy over just the pointers
		for ( i=0; i<elites_count && i<(int)this->chromosomes.size(); i++) {
			elites->push_back( this->chromosomes[i] );
		}
		
		//	Then, make them full copies (uses threads)
		this->CopyChromosomes(elites, population_new);
	}
	
	//
	void Population::EvaluateFitness_Thread(
		std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes,
		std::function<double(std::shared_ptr<Chromosome>)> evaluation_callback
	)
	{
		//
		std::shared_ptr<Chromosome> chromosome;
		double fitness;
		
		//
		while (true)
		{
			//	Grab a free chromosome
			this->evaluate_fitness_mutex.lock();
			chromosome = nullptr;
			if ( _chromosomes->size() ) {
				chromosome = _chromosomes->at(_chromosomes->size()-1);
				_chromosomes->pop_back();
			}
			this->evaluate_fitness_mutex.unlock();
			
			//	Call the evaluation callback
			if ( chromosome != nullptr ) {
				fitness = evaluation_callback(chromosome);
				chromosome->SetFitness(fitness);
			}
			
			//	We're done if there was nothing to grab
			else{
				break;
			}
		}
	}
	
	//
	void Population::EvaluateError_Thread(
		std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes,
		std::function<double(std::shared_ptr<Chromosome>)> evaluation_callback
	)
	{
		//
		std::shared_ptr<Chromosome> chromosome;
		double error;
		
		//
		while (true)
		{
			//	Grab a free chromosome
			this->evaluate_fitness_mutex.lock();
			chromosome = nullptr;
			if ( _chromosomes->size() ) {
				chromosome = _chromosomes->at(_chromosomes->size()-1);
				_chromosomes->pop_back();
			}
			this->evaluate_fitness_mutex.unlock();
			
			//	Call the evaluation callback
			if ( chromosome != nullptr ) {
				error = evaluation_callback(chromosome);
				chromosome->SetError(error);
			}
			
			//	We're done if there was nothing to grab
			else{
				break;
			}
		}
	}
	
	//
	void Population::CopyChromosomes(
		std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes_source,
		std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes_destination
	)
	{
		//
		std::vector<std::shared_ptr<std::thread>> threads;
		std::shared_ptr<std::thread> thread;
		int
			threads_count,
			i
			;
		
		//	Spawn threads
		threads_count = this->GetThreadCountSuggestion();
		for ( i=0; i<threads_count; i++) {
			
			//
			thread = std::shared_ptr<std::thread>(
				new std::thread(&Population::CopyChromosomes_Thread, this, _chromosomes_source, _chromosomes_destination)
			);
			threads.push_back(thread);
		}
		
		//	Wait for threads to finish
		for ( i=0; i<threads_count; i++ ) {
			threads[i]->join();
		}
	}
	
	//
	void Population::CopyChromosomes_Thread(
		std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes_source,
		std::shared_ptr<std::vector<std::shared_ptr<Chromosome>>> _chromosomes_destination
	)
	{
		//
		std::shared_ptr<Chromosome>
			chromosome_original,
			chromosome_copied
			;
		stringstream ss;
		
		//
		while ( _chromosomes_destination->size() < _chromosomes_source->size() )
		{
			//	Grab the next slot
			this->copy_chromosomes_mutex.lock();
			chromosome_original = nullptr;
			chromosome_copied = nullptr;
			if ( _chromosomes_destination->size() < _chromosomes_source->size() ) {
				
				//
				chromosome_copied = std::shared_ptr<Chromosome>(
					new Chromosome(this->random, 0)
				);
				_chromosomes_destination->push_back(chromosome_copied);
				chromosome_original = _chromosomes_source->at(_chromosomes_destination->size()-1);
			}
			this->copy_chromosomes_mutex.unlock();
			
			//	Make a full copy of the original, outside the lock
			if ( chromosome_copied && chromosome_original ) {
				*chromosome_copied = *chromosome_original;
			}
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
		mama = this->roulette_wheel->Spin();
		papa = this->roulette_wheel->Spin();
		
		//
		kiddo = this->breeder->Breed(
			mama, papa,
			this->crossover_type,
			this->crossover_order,
			this->crossover_bounds,
			this->crossover_point,
			this->crossover_point_std,
			this->mutation_rate
		);
		
		return kiddo;
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









