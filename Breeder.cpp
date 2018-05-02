

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
		Enums::CrossoverOrder crossover_order,
		Enums::CrossoverBounds crossover_bounds,
		double crossover_point,
		double crossover_point_std,
		double mutation_rate
	)
	{
		//
		std::shared_ptr<Chromosome>
			parent_primary,
			parent_secondary,
			kiddo
			;
		
		//	Choose primary / secondary parents
		switch( crossover_order )
		{
			//
			case Enums::CrossoverOrder::MamaPapa:
				parent_primary = mama;
				parent_secondary = papa;
				break;
			
			//
			case Enums::CrossoverOrder::ByFitness:
				if ( mama->GetFitness() > papa->GetFitness() ) {
					parent_primary = mama;
					parent_secondary = papa;
				}
				else{
					parent_primary = papa;
					parent_secondary = mama;
				}
				break;
		}
		
		//	Directly copy the primary parent first
		kiddo = std::shared_ptr<Chromosome>(new Chromosome(this->random, parent_primary->GetBitCount()));
		*kiddo = *parent_primary;
		
		//	Apply crossover with the secondary parent
		this->ApplyCrossover(
			kiddo, parent_secondary,
			crossover_type, crossover_bounds, crossover_point, crossover_point_std
		);
		
		//	Apply mutation
		this->Mutate(kiddo, mutation_rate);
		
		//	Reset kiddo's fitness
		kiddo->ResetFitness();
		
		//	Increment kiddo's generation number
		kiddo->IncrementGenerationNumber();
		
		return kiddo;
	}
	
	//
	void Breeder::Mutate(std::shared_ptr<Chromosome> chromosome, double mutation_rate)
	{
		//
		int
			i,
			size
			;
		
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
	int Breeder::PickRandomCrossoverPoint(
		std::shared_ptr<Chromosome> chromosome,
		Enums::CrossoverBounds crossover_bounds,
		double crossover_point,
		double crossover_point_std
	)
	{
		//
		int crossover_point_index;
		int bit_count;
		double random_double;
		
		//
		bit_count = chromosome->GetBitCount();
		
		/**
			Choose a double between [0.0,1.0] for the crossover point.
			Use normal distribution, with the mean and std from the parameters.
			That way, there is still randomness to the crossover point,
				but it still generally sticks near the chosen point.
		*/
		random_double = this->random->GetNormal(crossover_point, crossover_point_std);
		
		//	Apply to the actual int length
		crossover_point_index = floor(random_double * bit_count);
		
		//	Loop around to keep in bounds?
		if ( crossover_bounds == Enums::CrossoverBounds::Wrap ) {
			while ( crossover_point_index < 0 )
			{
				crossover_point_index += bit_count;
			}
			while ( crossover_point_index >= bit_count)
			{
				crossover_point_index -= bit_count;
			}
		}
		else if ( crossover_bounds == Enums::CrossoverBounds::Clip ) {
			if ( crossover_point_index < 0 ) {
				crossover_point_index = 0;
			}
			else if ( crossover_point_index >= bit_count ) {
				crossover_point_index = bit_count-1;
			}
		}
		else{
			throw std::runtime_error("Breeder::PickRandomCrossoverPoint() - Invalid crossover_bounds");
		}
		
		return crossover_point_index;
	}
	
	//
	void Breeder::ApplyCrossover(
		std::shared_ptr<Chromosome> kiddo,
		std::shared_ptr<Chromosome> parent,
		Enums::CrossoverType crossover_type,
		Enums::CrossoverBounds crossover_bounds,
		double crossover_point,
		double crossover_point_std
	)
	{
		//
		int
			bits_count,
			crossover_point_index,
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
		crossover_point_index = this->PickRandomCrossoverPoint(kiddo, crossover_bounds, crossover_point, crossover_point_std);
		
		//	Begin copying the parent at the crossover point and beyond
		//	(not before)
		for ( i=crossover_point_index; i<bits_count; i++) {
			kiddo->SetBit( i, parent->GetBit(i) );
		}
		
		//
		kiddo->ResetFitness();
	}
};







