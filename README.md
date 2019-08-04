
# Mike's Genetic Bit String Evolver

This library is intended to help you apply a genetic algorithm to any number of your own projects.

## License

This software is licensed under [GPLv3](./LICENSE)

## How to Use

The general process is quite simple:

1. Decide what *thing* of your own you'd like to evolve. This could be anything that can be represented with a binary string:
	* Simple floating point numbers or integers
	* Neural networks
	* Instructions that come together to make a program of some sort (ie LISP)
	* Anything you can imagine you'd like to evolve using a genetic algorothm

2. Write an *encoder* function which will convert your *thing* into a string of bits

3. Write a *decoder* function which will convert a string of bits into your *thing*

4. Write a *fitness* function that will give your *thing* a score based on how well it performs (using whatever arbitrary rules and tests you care to).

5. Include this library in your program; Use the Population classes to manage and evolve your Chromosome classes.

6. A typical workflow where you repeatedly evolve your *things* could be something like:

	1. Create a population using the Population class
	2. Use the Population class to grab individuals of the Chromosome class
	3. Grab each Chromosome's bit string with the method Chromosome::ToString()
	4. Decode each bit string to an instance of one of your *things*, keeping track of which Chromosome it was decoded from
	5. Run whatever tests you want, in order to determine the *fitness* of each of your *thing* instances (fitness is just a number; the higher the better).
	6. Iterate over all your *thing* instances and tell their linked Chromosome about the fitness you just calculated
	7. Use the Population class to perform an evolution of all the Chromosome classes at once with the method Population::Evolve()
	8. Delete all your *thing* instances, since they now refer to the previous generation of Chromosome instances that no longer exist
	9. Go back to Step 2 and keep repeating as long as you're seeing improvement.

Note that you don't actually need to write an encoder if you decide to do it this way.

### Encoder / Decoder Examples

**Simple Example**
Suppose you wanted to evolve an unsigned integer using only one byte (ranging from 0 to 255). Suppose at some point your unsigned integer is the number 11. Your *encoder* should produce the bit string **00001011**, since that's just 11 in binary. Your **decoder** should be able to turn **00001011** back into the number 11.

**Slightly Less Simple Example**
Suppose you wanted to evolve a program. For simplicity, we'll start by saying this is a simple language that can only *print* and *exit*. You might represent this by saying an instruction starts with four bits, followed by an eight bit number. Suppose we assign *0000* to the instruction *exit*, and *0001* to the instruction *print*. Now take the following simple program:

```
print 5
exit 0
```

Our encoder would then represent this program with the following bit string:

```000100000101000000000000```

##	Features

The following features have been implemented thus far:

* **Sexual Reproduction**
	Two parent Chromosomes are selected, and a simple crossover operator is used to combine their bits into a single child Chromosome, before applying mutation.

* **Asexual Reproduction**
	One parent Chromosome duplicates itself exactly into a child Chromosome, before applying mutation

* **Roulette Wheel Selection**
	When choosing parent Chromosomes to produce offspring, a method is used to allow Chromosomes that have higher fitness to reproduce more often, yet without guaranteeing any fit Chromosome will reproduce. Less fit Chromosomes will have less chance to reproduce, but aren't guaranteed *not* to reproduce. 

* **Elitism**
	Before evolving a population of Chromosomes, a few of the *most fit* Chromosomes are copied directly into the next population without modification, in an attempt to ensure that a population's *Champions* (most fit Chromosomes) never get worse as a result of evolution. Elitism has two modes, currently:
	* *Rate* : The number of *champions* is determined by taking a percentage of the total population. A rate of 2% with a population size of 200 would result in 4 *champions*
	* *Absolute* : The number of *champions* can be set to any whole number manually







