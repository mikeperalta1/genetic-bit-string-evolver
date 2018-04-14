

//
#include "BitEvolver/Random.h"
#include "BitEvolver/Chromosome.h"
#include "BitEvolver/Breeder.h"


//
#include <iostream>


//
namespace BitEvolver
{
	//
	using std::cout;
	using std::endl;
	
	//
	Breeder::Breeder(std::shared_ptr<Random> _random)
	{
		//
		this->random = _random;
	}
	
	//
	std::shared_ptr<Chromosome> Breeder::Breed(
		std::shared_ptr<Chromosome> mama,
		std::shared_ptr<Chromosome> papa,
		Enums::CrossoverType crossover_type,
		double crossover_rate,
		double mutation_rate
	)
	{
		//
		std::shared_ptr<Chromosome> kiddo;
		int bit_length;
		
		//
		bit_length = mama->GetBitCount();
		
		//	Directly copy the mama
		kiddo = std::shared_ptr<Chromosome>(new Chromosome(this->random, bit_length));
		*kiddo = *mama;
		
		//	Apply crossover
		this->ApplyCrossover(kiddo, papa, crossover_type, crossover_rate);
		
		//	Apply mutation
		this->Mutate(kiddo, mutation_rate);
		
		//	Reset kiddo's fitness
		kiddo->ResetFitness();
		
		return kiddo;
	}
	
	//
	void Breeder::Mutate(std::shared_ptr<Chromosome> chromosome, double mutation_rate)
	{
		//
		int i, size;
		
		//
		#warning "For efficiency, let's instead roll the number of bits to flip in advance, and loop that many times (faster than rolling for every bit)"
		
		//
		size = chromosome->GetBitCount();
		for ( i=0; i<size; i++ ) {
			
			//
			if ( this->random->RollBool(mutation_rate) ) {
				chromosome->FlipBit(i);
			}
		}
		
		//
		chromosome->ResetFitness();
	}
	
	//
	int Breeder::PickRandomCrossoverPoint(std::shared_ptr<Chromosome> chromosome, double crossover_rate)
	{
		//
		double crossover_point_double;
		int crossover_point;
		int bit_count;
		
		//
		bit_count = chromosome->GetBitCount();
		
		/**
			Choose a double between [0.0,1.0] for the crossover point.
			Use normal distribution, with the mean to the default point,
				and 1.0 as the std.
			That way, there is still randomness to the crossover point,
				but it still generally sticks near the default
		*/
		crossover_point_double = this->random->GetNormal(crossover_rate, 1.0);
		
		//	Apply to the actual int length
		crossover_point = crossover_point_double * bit_count;
		
		//	Loop around to keep in bounds
		while ( crossover_point < 0 )
		{
			crossover_point += bit_count;
		}
		while ( crossover_point >= bit_count)
		{
			crossover_point -= bit_count;
		}
		
		return crossover_point;
	}
	
	//
	void Breeder::ApplyCrossover(
		std::shared_ptr<Chromosome> kiddo,
		std::shared_ptr<Chromosome> parent,
		Enums::CrossoverType crossover_type,
		double crossover_rate
	)
	{
		//
		int
			bits_count,
			crossover_point,
			i
			;
		
		//	Only proceed if using sexual crossover
		if (crossover_type != Enums::CrossoverType::Sexual) {
			return;
		}
		
		//	For now, don't crossover unless the bit lengths are identical
		bits_count = kiddo->GetBitCount();
		if ( parent->GetBitCount() != bits_count ) {
			throw std::runtime_error("Breeder::ApplyCrossover() - Parent incompatible with Kiddo (bit lengths don't match)");
		}
		
		//	Pick random crossover point
		crossover_point = this->PickRandomCrossoverPoint(kiddo, crossover_rate);
		
		//	Begin copying the parent at the crossover point and beyond
		//	(not before)
		for ( i=crossover_point; i<bits_count; i++) {
			kiddo->SetBit( i, parent->GetBit(i) );
		}
		
		//
		kiddo->ResetFitness();
	}
};







