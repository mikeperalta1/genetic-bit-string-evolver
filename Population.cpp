

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
		this->SetCrossoverType(Population::DEFAULT_CROSSOVER_TYPE);
		this->SetCrossoverPoint(Population::DEFAULT_CROSSOVER_POINT);
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
		std::shared_ptr<RouletteWheel> wheel;
		std::vector<std::shared_ptr<std::thread>> threads;
		std::shared_ptr<std::thread> thread;
		int
			thread_count,
			i
			;
		
		//	First, populate the roulette wheel
		this->roulette_wheel->SetChromosomes(this->chromosomes);
		
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
		//cout << "Roulette Wheel (mama): " << mama->ToString() << endl;
		//cout << "Roulette Wheel (papa): " << papa->ToString() << endl;
		
		//
		kiddo = this->breeder->Breed(
			mama, papa,
			this->crossover_type,
			this->crossover_point,
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









