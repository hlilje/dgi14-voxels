#include "../include/PerlinNoise.hpp"

PerlinNoise::PerlinNoise(unsigned int seed)
{
	p.resize(256);

	// Fill p with values from 0 to 255
	std::iota(p.begin(), p.end(), 0);

	// Initialize a random engine with seed
	//std::default_random_engine engine(seed);

	// Shuffle using the above random engine
	//std::shuffle(p.begin(), p.end(), engine);

	// Duplicate the permutation vector
	p.insert(p.end(), p.begin(), p.end());
}
