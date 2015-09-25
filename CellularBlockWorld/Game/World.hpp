#ifndef included_World
#define included_World
#pragma once

//-------------------------------------------------------------
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>
#include "../Engine/Math/MyMathFiles.hpp"
#include "../Engine/Constants.hpp"
#include "../Engine/Camera/Camera.hpp"
#include "../Engine/Time/Time.hpp"
#include "../Engine/Renderer/OpenGLRenderer.hpp"
#include "CubeCell.hpp"
#include "Genome.hpp"

//-------------------------------------------------------------
class World
{
public:
	World();
	bool ProcessKeyDownEvent( unsigned char virtualKeyCode);
	bool ProcessKeyUpEvent( unsigned char virtualKeyCode);
	void UpdatePlayerFromInput( float deltaseconds );
	void UpdateCameraFromInput( float deltaseconds );
	void UpdateFromMouseInput();
	void FindCameraFacingVector();
	void CheckForGimbleLock();
	void Initialize();
	void BuildZone2D();
	void BuildZone3D();

	void GameOfLifeCellularAutomataPass2D();
	void GameOfLifeCellularAutomataPass3D();

	void GetAllCellsInRayTrace();

	void Update();
	void Render();

	bool m_isKeyDown[ 256 ];
	std::vector<CubeCell> m_worldBlockCells;
	bool m_keyIsHeld;

	std::vector<CubeCell> m_temporaryCellularVector;

	std::vector<Genome> m_genomesForTesting;
	Genome* m_currentGenome;

	Camera m_camera;
	OpenGLRenderer m_renderer;

	float m_mousePositionXDifference;
	float m_mousePositionZDifference;

	bool m_is2DWorld;
	bool m_isPaused;

	double m_lastCellularAutomataTime;
};

#endif //included_World