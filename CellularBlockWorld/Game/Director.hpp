#ifndef included_Director
#define included_Director
#pragma once

//-------------------------------------------------------------
#include "Population.hpp"

//-------------------------------------------------------------
class Director
{
public:
	Director();

	void AssignFitnessScore();
	void CreateNextPopulation();

	Population m_firstGenomeBuffer;
	Population m_secondGenomeBuffer;
	Population m_genomeHallofFame;

};

#endif //included_Director