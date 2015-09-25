#ifndef included_CubeCell
#define included_CubeCell
#pragma once

//-------------------------------------------------------------


//-------------------------------------------------------------
class CubeCell
{
public:
	CubeCell()
	{
		m_lifeValue = 0.f;
	};
	CubeCell(float lifeValue)
	{
		m_lifeValue = lifeValue;
	};

	float m_lifeValue;
};

#endif //included_CubeCell