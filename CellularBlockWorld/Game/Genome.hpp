#ifndef included_Genome
#define included_Genome
#pragma once

//-------------------------------------------------------------


//-------------------------------------------------------------
class Genome
{
public:
	Genome()
	{
		m_maxStarveLossPerStep = 0.f;
		m_maxCrowdLossPerStep = 0.f;
		m_maxBirthsPerStep = 0.f;
		m_starveThreshold = 0.f;
		m_crowdThreshold = 0.f;
		m_stableThreshold = 0.f;

		m_initialLifePercentageInWorld = 0;
		m_fitnessScore = 0.f;
	};

	Genome(float starveLoss, float crowdLoss, float birthGain, float starveThreshold, float crowdThreshold, float stableThreshold, int lifeRatio)
	{
		m_maxStarveLossPerStep = starveLoss;
		m_maxCrowdLossPerStep = crowdLoss;
		m_maxBirthsPerStep = birthGain;
		m_starveThreshold = starveThreshold;
		m_crowdThreshold = crowdThreshold;
		m_stableThreshold = stableThreshold;

		m_initialLifePercentageInWorld = lifeRatio;

		m_fitnessScore = 0.f;
	};

	float GetMaxBirthThreshold() const
	{
		return (m_stableThreshold + m_starveThreshold) * 0.5f;
	}
	
	float m_maxStarveLossPerStep;
	float m_maxCrowdLossPerStep;
	float m_maxBirthsPerStep;
	float m_starveThreshold;
	float m_crowdThreshold;
	float m_stableThreshold;

	int m_initialLifePercentageInWorld;

	float m_fitnessScore;
	
	/*
		Max starve loss per step
		Max crowding loss per step
		Max births life per step
		Starve threshold
		Crowd threshold
		Stable threshold
		Initial ratio of living cells
	*/
};

inline bool operator < (const Genome& first, const Genome& second)
{
	if (first.m_fitnessScore < second.m_fitnessScore) {
		return true;
	}
	else {
		return false;
	}
};

#endif //included_Genome