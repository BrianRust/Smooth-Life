#ifndef included_Population
#define included_Population
#pragma once

//-------------------------------------------------------------
#include "../Engine/Constants.hpp"
#include "Genome.hpp"
#include <vector>

//-------------------------------------------------------------
class Population
{
public:
	Population();

	void Initialize();
	void SortPopulation();

	std::vector<Genome> m_genomePopulation;
	//Genome m_genomePopulation[ConstantParameters::POPULATION_SIZE];
	Genome* m_currentGenome;
};

#endif //included_Population