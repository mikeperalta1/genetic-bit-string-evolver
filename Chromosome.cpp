

//
#include "BitEvolver/Chromosome.h"
#include "BitEvolver/Random.h"


//
#include <memory>
#include <string>
#include <sstream>
#include <vector>


//
namespace BitEvolver
{
	//
	using std::string;
	using std::to_string;
	using std::stringstream;
	
	//
	Chromosome::Chromosome(std::shared_ptr<Random> _random, int _bits)
	{
		//
		this->random = _random;
		this->SetBitCount(_bits);
		
		//
		this->Reset();
	}
	
	//
	void Chromosome::Reset()
	{
		//
		this->Randomize();
	}
	
	//
	void Chromosome::Randomize()
	{
		//
		int i;
		
		//
		this->bits.clear();
		
		//
		for ( i=0; i<this->bits_count_desired; i++ ) {
			this->bits.push_back(this->random->GetInt(0, 1));
		}
	}
	
	//
	void Chromosome::SetBitCount(int count)
	{
		//
		this->bits_count_desired = count;
		this->Randomize();
	}
	
	//
	int Chromosome::GetBitCount()
	{
		return (int)this->bits.size();
	}
	
	//
	void Chromosome::FlipBit(int index)
	{
		//
		if ( index >= (int)this->bits.size() ) {
			throw std::runtime_error("Chromosome::FlipBit() - Tried to flip out of range bit index: " + to_string(index));
		}
		
		//
		if ( this->bits[index] ) {
			this->bits[index] = false;
		}
		else{
			this->bits[index] = true;
		}
	}
	
	//
	bool Chromosome::GetBit(int index)
	{
		//
		if ( index >= (int)this->bits.size() ) {
			throw std::runtime_error("Chromosome::GetBit() - Tried to access out of bounds bit");
		}
		
		//
		return this->bits[index];
	}
	
	//
	void Chromosome::SetBit(int index, bool b)
	{
		//
		if ( index >= (int)this->bits.size() ) {
			throw std::runtime_error("Chromosome::GetBit() - Tried to access out of bounds bit");
		}
		
		//
		this->bits[index] = b;
	}
	
	//
	void Chromosome::ResetFitness()
	{
		//
		this->SetFitness(0.0);
	}
	
	//
	void Chromosome::SetFitness(double d)
	{
		//
		this->fitness = d;
	}
	
	//
	void Chromosome::AdjustFitness(double d)
	{
		//
		this->fitness += d;
	}
	
	//
	double Chromosome::GetFitness()
	{
		//
		return this->fitness;
	}
	
	//
	string Chromosome::ToString()
	{
		//
		stringstream s;
		
		//
		for ( bool b : this->bits ) {
			
			//
			if ( b ) {
				s << "1";
			}
			else{
				s << "0";
			}
		}
		
		//
		return s.str();
	}
	
	//
	const Chromosome& Chromosome::operator=(const Chromosome& other)
	{
		//
		int i;
		
		//
		this->random = other.random;
		this->bits = other.bits;
		this->bits_count_desired = other.bits_count_desired;
		this->fitness = other.fitness;
		
		//	Copy the bits!
		this->bits.clear();
		for ( i=0; i<(int)other.bits.size(); i++ ) {
			this->bits.push_back(other.bits[i]);
		}
		
		return *this;
	}
};



