#ifndef BITEVOLVER_ENUMS_H
#define BITEVOLVER_ENUMS_H


//
namespace BitEvolver
{
	//
	namespace Enums
	{
		//
		enum class CrossoverType
		{
			//
			None,
			Sexual
		};
		
		//
		enum class CrossoverOrder
		{
			//
			MamaPapa,
			ByFitness
		};
		
		//
		enum class CrossoverBounds
		{
			//
			Clip,
			Wrap
		};
		
		//
		enum class ElitismType
		{
			//
			None,
			Rate,
			Absolute
		};
	};
};


#endif