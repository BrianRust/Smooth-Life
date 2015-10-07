#include "Population.hpp"
#include <algorithm>

//-----------------------------------------
Population::Population() {
	
}

//-----------------------------------------
void Population::Initialize()
{
	for (unsigned int index = 0; index < ConstantParameters::POPULATION_SIZE; index++) {
		m_genomePopulation.push_back(Genome());
		m_genomePopulation[index].m_crowdThreshold = 0.f;
		m_genomePopulation[index].m_maxBirthsPerStep = 0.f;
		m_genomePopulation[index].m_maxCrowdLossPerStep = 0.f;
		m_genomePopulation[index].m_maxStarveLossPerStep = 0.f;
		m_genomePopulation[index].m_stableThreshold = 0.f;
		m_genomePopulation[index].m_starveThreshold = 0.f;

		m_genomePopulation[index].m_initialLifePercentageInWorld = 50;
	}

	m_currentGenome = &m_genomePopulation[0];
}

//-----------------------------------------
void Population::SortPopulation()
{
	std::sort( m_genomePopulation.begin(), m_genomePopulation.end() );
}


