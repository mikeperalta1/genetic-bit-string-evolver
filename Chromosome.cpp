

//
#include "BitEvolver/Chromosome.h"
#include "BitEvolver/Random.h"


//
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <mutex>


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
		
		//
		this->generation_number = 1;
		
		//
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
		std::unique_lock<std::recursive_mutex> lock(this->modification_mutex);
		int i;
		
		//
		this->bits.clear();
		
		//
		for ( i=0; i<this->bits_count_desired; i++ ) {
			this->bits.push_back(this->random->GetInt(0, 1));
		}
	}
	
	//
	void Chromosome::SetGenerationNumber(int g)
	{
		//
		this->generation_number = g;
	}
	
	//
	void Chromosome::IncrementGenerationNumber()
	{
		//
		this->generation_number++;
	}
	
	//
	int Chromosome::GetGenerationNumber()
	{
		//
		return this->generation_number;
	}
	
	//
	void Chromosome::SetBitCount(int count)
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->modification_mutex);
		
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
		std::unique_lock<std::recursive_mutex> lock(this->modification_mutex);
		
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
		std::unique_lock<std::recursive_mutex> lock(this->modification_mutex);
		
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
		std::unique_lock<std::recursive_mutex> lock(this->modification_mutex);
		
		//
		if ( index >= (int)this->bits.size() ) {
			throw std::runtime_error("Chromosome::GetBit() - Tried to access out of bounds bit");
		}
		
		//
		this->bits[index] = b;
	}
	
	//
	void Chromosome::SetBits(string s)
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->modification_mutex);
		size_t i;
		char c;
		
		//
		this->SetBitCount( (int) s.size() );
		
		//
		for ( i=0; i<s.size(); i++ ) {
			
			//
			c = s[i];
			if ( c == '1' ) {
				this->bits[i] = true;
			}
			else if ( c == '0' ) {
				this->bits[i] = false;
			}
			else{
				stringstream ss;
				ss << "Chromosome::SetBits() - Invalid character '" << c << "' (" << (int)c << ") in bit string input";
				throw std::runtime_error( ss.str() );
			}
		}
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
	void Chromosome::ResetError()
	{
		//
		this->ResetFitness();
	}
	
	//
	void Chromosome::SetError(double e)
	{
		//
		this->SetFitness(-e);
	}
	
	//
	void Chromosome::AdjustError(double e)
	{
		//
		this->AdjustFitness(-e);
	}
	
	//
	double Chromosome::GetError()
	{
		//
		return -this->GetFitness();
	}
	
	//
	string Chromosome::ToString()
	{
		//
		std::unique_lock<std::recursive_mutex> lock(this->modification_mutex);
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
		std::unique_lock<std::recursive_mutex> lock1(this->modification_mutex);
		int i;
		
		//
		this->random = other.random;
		this->generation_number = other.generation_number;
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



