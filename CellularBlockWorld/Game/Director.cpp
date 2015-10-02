#include "Director.hpp"

const int QUARTER_OF_POPULATION = ConstantParameters::POPULATION_SIZE / 4;

//-------------------------------------------
Director::Director() 
	: m_firstGenomeBuffer(Population())
	, m_secondGenomeBuffer(Population())
	, m_genomeHallofFame(Population())
	, m_currentGenomeBuffer(&m_firstGenomeBuffer)
{
	
}

//------------------------------------------
void Director::AssignFitnessScore()
{

}

//------------------------------------------
void Director::CreateNextPopulation()
{
	int num_GenomesRemaining = ConstantParameters::POPULATION_SIZE;
	
	m_firstGenomeBuffer.SortPopulation();
	m_secondGenomeBuffer.m_genomePopulation.clear();
	
	int currentQuarter = 1;
	for ( int index = m_firstGenomeBuffer.m_genomePopulation.size()-1; index > 0; index-- ) {
		
		switch(currentQuarter) {
		case 1:
			//take top quarter and keep
			m_genomeHallofFame.m_genomePopulation.push_back( m_firstGenomeBuffer.m_genomePopulation[index] );
			m_secondGenomeBuffer.m_genomePopulation.push_back( m_firstGenomeBuffer.m_genomePopulation[index] );
			break;
		case 2:
			//take next quarter and mutate

			break;
		case 3:
			//take next quarter and breed with the keepers

			break;
		default:
			//take the remaining and randomize

			break;
		}
	}
}
