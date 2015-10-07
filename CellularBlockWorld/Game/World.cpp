#include "World.hpp"
#include "cmath"
#include "../Engine/Math/MathUtils.hpp"

static bool leftMouseButtonDown = false;
static bool rightMouseButtonDown = false;

//----------------------------------------------------
World::World(  ) 
	: m_mousePositionXDifference(0.f)
	, m_mousePositionZDifference(0.f)
	, m_is2DWorld(false)
	, m_isPaused(true)
	, m_lastCellularAutomataTime(0.0)
	, m_currentGenome(nullptr)
{
	srand ((unsigned int)(time(NULL)));

	m_camera = Camera();
	m_renderer = OpenGLRenderer();
	m_keyIsHeld = false;
}

//----------------------------------------------------
void World::Initialize() {
	m_genomesForTesting.push_back(Genome(0.5f, 0.5f, 0.5f, 0.1875f, 0.4375f, 0.3125f, ConstantParameters::SOLID_BLOCK_PERCENTAGE_2D));
	m_currentGenome = &m_genomesForTesting[0];
	
	for (unsigned int index = 0; index < ConstantParameters::TOTAL_BLOCKS_IN_ZONE; index++) {
		m_worldBlockCells.push_back(CubeCell(false));
	}
	BuildZone3D();
}

//----------------------------------------------------
void World::BuildZone2D() 
{
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int z = 0;

	//m_solidBlocks.reserve(ConstantParameters::TOTAL_BLOCKS_IN_ZONE);
	//m_temporaryCellularVector.reserve(ConstantParameters::TOTAL_BLOCKS_IN_ZONE);

	int counter = 0;

	for (unsigned int index = 0; index < ConstantParameters::WIDTH_TIMES_HEIGHT; index++) 
	{
		if ( (rand() % 100) < m_currentGenome->m_initialLifePercentageInWorld ) 
		{
			x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
			y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
			z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;

			m_worldBlockCells[index].m_lifeValue = 1.f;
			m_renderer.AddCubeToBuffer(Vector3((float) x, (float) y, (float) z), 1.f);

			counter++;
		}
		else 
		{
			m_worldBlockCells[index].m_lifeValue = 0.f;
		}

		m_temporaryCellularVector.push_back((CubeCell(0.f)));
	}

	m_renderer.PushCubeVerticesToVBO();
}

//----------------------------------------------------
void World::BuildZone3D() 
{
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int z = 0;

	//m_solidBlocks.clear();
	//m_temporaryCellularVector.clear();

	//m_solidBlocks.reserve(ConstantParameters::TOTAL_BLOCKS_IN_ZONE);
	m_temporaryCellularVector.reserve(ConstantParameters::TOTAL_BLOCKS_IN_ZONE);

	int counter = 0;

	for (unsigned int index = 0; index < ConstantParameters::TOTAL_BLOCKS_IN_ZONE; index++) 
	{
		if ( (rand() % 100) < m_currentGenome->m_initialLifePercentageInWorld ) 
		{
			x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
			y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
			z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;

			m_worldBlockCells[index].m_lifeValue = 1.f;
			m_renderer.AddCubeToBuffer(Vector3((float) x, (float) y, (float) z), 1.f);
			counter++;
		}
		else 
		{
			m_worldBlockCells[index].m_lifeValue = 0.f;
		}

		m_temporaryCellularVector.push_back((CubeCell(0.f)));
	}

	m_renderer.PushCubeVerticesToVBO();
}

//----------------------------------------------------
bool World::ProcessKeyDownEvent( unsigned char virtualKeyCode) 
{
	m_isKeyDown[ virtualKeyCode ] = true;
	return true;
}

//----------------------------------------------------
bool World::ProcessKeyUpEvent( unsigned char virtualKeyCode) 
{
	m_isKeyDown[ virtualKeyCode ] = false;
	return true;
}

//----------------------------------------------------
void World::UpdateCameraFromInput( float deltaSeconds ) 
{
	m_camera.m_cameraPitch -= deltaSeconds * m_mousePositionZDifference * ConstantParameters::MOUSE_SPEED;
	m_camera.m_cameraYaw += deltaSeconds * m_mousePositionXDifference * ConstantParameters::MOUSE_SPEED;
}

//----------------------------------------------------
void World::FindCameraFacingVector() 
{
	m_camera.m_cameraFacingVector.x = cos(m_camera.m_cameraYaw) * cos(m_camera.m_cameraPitch);
	m_camera.m_cameraFacingVector.y = sin(m_camera.m_cameraYaw) * cos(m_camera.m_cameraPitch);
	m_camera.m_cameraFacingVector.z = -sin(m_camera.m_cameraPitch);
}

//----------------------------------------------------
void World::UpdatePlayerFromInput( float deltaSeconds ) 
{	
	if ( m_isKeyDown[ 'P' ] && !m_keyIsHeld ) 
	{
		m_keyIsHeld = true;
		m_isPaused = !m_isPaused;
	}
	
	if ( m_isKeyDown[ 'T' ] && !m_keyIsHeld ) 
	{
		m_keyIsHeld = true;
		m_renderer.DeleteBuffers();
		if (m_is2DWorld) 
		{
			GameOfLifeCellularAutomataPass2D();
		} 
		else 
		{
			GameOfLifeCellularAutomataPass3D();
		}
	}

	if ( m_isKeyDown[ 'R' ] && !m_keyIsHeld ) 
	{
		m_keyIsHeld = true;
		m_renderer.DeleteBuffers();
		BuildZone3D();
		m_is2DWorld = false;
	}

	if ( m_isKeyDown[ 'E' ] ) 
	{
		m_renderer.DeleteBuffers();
		BuildZone2D();
		m_is2DWorld = true;
	}
	
	if (m_isKeyDown[ VK_LEFT ]) 
	{
		m_camera.m_cameraYaw += deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ VK_RIGHT ]) 
	{
		m_camera.m_cameraYaw -= deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ VK_UP ]) 
	{
		m_camera.m_cameraPitch -= deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ VK_DOWN ]) 
	{
		m_camera.m_cameraPitch += deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ 'W' ]) 
	{
		m_camera.m_cameraPosition.x += cos(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
		m_camera.m_cameraPosition.y += sin(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ 'D' ]) 
	{
		m_camera.m_cameraPosition.y -= cos(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
		m_camera.m_cameraPosition.x += sin(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ 'S' ]) 
	{
		m_camera.m_cameraPosition.x -= cos(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
		m_camera.m_cameraPosition.y -= sin(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ 'A' ]) 
	{
		m_camera.m_cameraPosition.y += cos(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
		m_camera.m_cameraPosition.x -= sin(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ VK_SPACE ]) 
	{
		m_camera.m_cameraPosition.z += deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[VK_CONTROL]) 
	{
		m_camera.m_cameraPosition.z -= deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if ( !( m_isKeyDown[ 'T' ] || m_isKeyDown[ 'R' ] || m_isKeyDown[ 'P' ]) ) 
	{
		m_keyIsHeld = false;
	}
}

//----------------------------------------------------
void World::CheckForGimbleLock() 
{
	if (m_camera.m_cameraPitch >= ConstantParameters::CAMERA_Z_MAX) 
	{
		m_camera.m_cameraPitch = ConstantParameters::CAMERA_Z_MAX - 0.005f;
	}

	if (m_camera.m_cameraPitch <= -ConstantParameters::CAMERA_Z_MAX) 
	{
		m_camera.m_cameraPitch = -ConstantParameters::CAMERA_Z_MAX + 0.005f;
	}
}

//----------------------------------------------------
void World::Update() 
{
	float currentTime = (float) Time::GetCurrentTimeSeconds();
	float deltaSeconds = ConstantParameters::DELTA_SECONDS; // Hack: assume 60 FPS

	UpdatePlayerFromInput( deltaSeconds );
	UpdateCameraFromInput( deltaSeconds );
	FindCameraFacingVector();
	CheckForGimbleLock();

	if (!m_isPaused) 
	{
		if ( ( currentTime - m_lastCellularAutomataTime ) > ConstantParameters::TIME_BETWEEN_CA_STEPS ) 
		{
			m_lastCellularAutomataTime = currentTime;
			if (m_is2DWorld) 
			{
				m_renderer.DeleteBuffers();
				GameOfLifeCellularAutomataPass2D();
			} 
			else 
			{
				m_renderer.DeleteBuffers();
				GameOfLifeCellularAutomataPass3D();
			}
		}
	}
}

//----------------------------------------------------
void World::Render() 
{	
	m_renderer.SetModelViewProjectionMatrix(m_camera);
	m_renderer.SendCubeVBO();
	m_renderer.PopMatrix();
}

//----------------------------------------------------
void World::GameOfLifeCellularAutomataPass2D() 
{
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int z = 0;

	float totalSurroundingLife = 0.f;
	float averageLife = 0.f;
	const float INVERSE_NUM_SURROUNDING_CELL_WEIGHTS = 1.f/6.84f;

	unsigned int surroundingCellIndices[NUM_NEIGHBORS];
	float surroundingCellSteps[NUM_NEIGHBORS];

	//set feathering values
	surroundingCellSteps[NEIGHBOR_WEST] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //left
	surroundingCellSteps[NEIGHBOR_SOUTH] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //bottom
	surroundingCellSteps[NEIGHBOR_SOUTHWEST] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //bottom left
	surroundingCellSteps[NEIGHBOR_NORTH] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //top
	surroundingCellSteps[NEIGHBOR_NORTHWEST] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //top left
	surroundingCellSteps[NEIGHBOR_EAST] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //right
	surroundingCellSteps[NEIGHBOR_SOUTHEAST] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //bottom right
	surroundingCellSteps[NEIGHBOR_NORTHEAST] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //top right
	
	for (unsigned int index = 0; index < ConstantParameters::WIDTH_TIMES_HEIGHT; index++) 
	{
		totalSurroundingLife = 0.f;
		averageLife = 0.f;

		x = index & ( ConstantParameters::BLOCKS_X_AXIS - 1 );
		y = (index >> ConstantParameters::BLOCKS_X_POWER) & ( ConstantParameters::BLOCKS_Y_AXIS - 1 );
		z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ( ConstantParameters::BLOCKS_Z_AXIS - 1 );

		//get surrounding indices
		if ( x > 0 ) 
		{
			surroundingCellIndices[NEIGHBOR_WEST] = index - 1; //left
		} 
		else 
		{
			surroundingCellIndices[NEIGHBOR_WEST] = ( index + ConstantParameters::BLOCKS_X_AXIS ) - 1; //left
		}

		if ( y > 0 ) 
		{
			surroundingCellIndices[NEIGHBOR_SOUTH] = index - ConstantParameters::BLOCKS_X_AXIS; //bottom
			surroundingCellIndices[NEIGHBOR_SOUTHWEST] = surroundingCellIndices[NEIGHBOR_WEST] - ConstantParameters::BLOCKS_X_AXIS; //bottom left
		} 
		else 
		{
			surroundingCellIndices[NEIGHBOR_SOUTH] = index + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //bottom
			surroundingCellIndices[NEIGHBOR_SOUTHWEST] = surroundingCellIndices[NEIGHBOR_WEST] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //bottom left
		}

		if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) 
		{
			surroundingCellIndices[NEIGHBOR_NORTH] = index + ConstantParameters::BLOCKS_X_AXIS; //top
			surroundingCellIndices[NEIGHBOR_NORTHWEST] = surroundingCellIndices[NEIGHBOR_WEST] + ConstantParameters::BLOCKS_X_AXIS; //top left
		} 
		else 
		{
			surroundingCellIndices[NEIGHBOR_NORTH] = index - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //top
			surroundingCellIndices[NEIGHBOR_NORTHWEST] = surroundingCellIndices[NEIGHBOR_WEST] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //top left
		}

		if ( x < ( ConstantParameters::BLOCKS_X_AXIS - 1 ) ) 
		{
			surroundingCellIndices[NEIGHBOR_EAST] = index + 1; //right
			surroundingCellIndices[NEIGHBOR_SOUTHEAST] = surroundingCellIndices[NEIGHBOR_SOUTH] + 1; //bottom right
			surroundingCellIndices[NEIGHBOR_NORTHEAST] = surroundingCellIndices[NEIGHBOR_NORTH] + 1; //top right
		} 
		else 
		{
			surroundingCellIndices[NEIGHBOR_EAST] = index - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //right
			surroundingCellIndices[NEIGHBOR_SOUTHEAST] = surroundingCellIndices[NEIGHBOR_SOUTH] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //bottom right
			surroundingCellIndices[NEIGHBOR_NORTHEAST] = surroundingCellIndices[NEIGHBOR_NORTH] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //top right
		}

		//get total surrounding life and average
		for ( unsigned int surroundingIndex = 0; surroundingIndex < NUM_NEIGHBORS; surroundingIndex++ ) 
		{
			unsigned int neighborIndex = surroundingCellIndices[surroundingIndex];
			float neighborLife = m_worldBlockCells[ neighborIndex ].m_lifeValue;
			float weightedNeighborLife = neighborLife* surroundingCellSteps[surroundingIndex];
			totalSurroundingLife += weightedNeighborLife;
		}
		averageLife = totalSurroundingLife * INVERSE_NUM_SURROUNDING_CELL_WEIGHTS;

		m_temporaryCellularVector[index].m_lifeValue = m_worldBlockCells[index].m_lifeValue;
		float lifeChange = 0.f;
		float maxBirthThreshold = m_currentGenome->GetMaxBirthThreshold();
		if ( averageLife < m_currentGenome->m_starveThreshold ) 
		{
			lifeChange = RangeMapFloat( averageLife, 0.f, m_currentGenome->m_starveThreshold, -1.f, 0.f );
		}
		else if ( averageLife > m_currentGenome->m_crowdThreshold )
		{
			lifeChange = RangeMapFloat( averageLife, m_currentGenome->m_crowdThreshold, 1.f, 0.f, -1.f );
		}
		else if ( averageLife > m_currentGenome->m_stableThreshold )
		{
			//No Change
		}
		else if ( averageLife < maxBirthThreshold )
		{
			lifeChange = RangeMapFloat( averageLife, m_currentGenome->m_starveThreshold, maxBirthThreshold, 0.f, 1.f );
		}
		else
		{
			lifeChange = RangeMapFloat( averageLife, maxBirthThreshold, m_currentGenome->m_stableThreshold, 1.f, 0.f );
		}

		m_temporaryCellularVector[index].m_lifeValue += lifeChange;
		m_temporaryCellularVector[index].m_lifeValue = ClampFloat(m_temporaryCellularVector[index].m_lifeValue, 0.f, 1.f);
	}

	for (unsigned int index = 0; index < ConstantParameters::WIDTH_TIMES_HEIGHT; index++) 
	{
		m_worldBlockCells[index] = m_temporaryCellularVector[index];

		if ( m_worldBlockCells[index].m_lifeValue > 0.f ) 
		{
			x = index & ( ConstantParameters::BLOCKS_X_AXIS - 1 );
			y = ( index >> ConstantParameters::BLOCKS_X_POWER ) & ( ConstantParameters::BLOCKS_Y_AXIS - 1 );
			z = ( index >> ConstantParameters::BLOCKS_XY_POWER ) & ( ConstantParameters::BLOCKS_Z_AXIS - 1 );	
			m_renderer.AddCubeToBuffer(Vector3((float) x, (float) y, (float) z), m_worldBlockCells[index].m_lifeValue);
		}
	}

	m_renderer.PushCubeVerticesToVBO();
}

//----------------------------------------------------
void World::GameOfLifeCellularAutomataPass3D() 
{
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int z = 0;

	float totalSurroundingLife = 0.f;
	float averageLife = 0.f;

	const float INVERSE_NUM_SURROUNDING_CELL_WEIGHTS = 1.f/19.16f;

	unsigned int surroundingCellIndices[26];
	float surroundingCellSteps[26];

	//set feathering values
	surroundingCellSteps[0] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //left
	surroundingCellSteps[1] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //bottom
	surroundingCellSteps[2] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //bottom left
	surroundingCellSteps[3] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //top
	surroundingCellSteps[4] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //top left
	surroundingCellSteps[5] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //right
	surroundingCellSteps[6] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //bottom right
	surroundingCellSteps[7] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //top right
	surroundingCellSteps[8] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //above
	surroundingCellSteps[9] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //below
	surroundingCellSteps[10] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //above left
	surroundingCellSteps[11] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //below left
	surroundingCellSteps[12] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //above bottom
	surroundingCellSteps[13] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //below bottom
	surroundingCellSteps[14] = ConstantParameters::THREE_STEP_CELL_DISTANCE; //above bottom left
	surroundingCellSteps[15] = ConstantParameters::THREE_STEP_CELL_DISTANCE; //below bottom left
	surroundingCellSteps[16] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //above top
	surroundingCellSteps[17] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //below top
	surroundingCellSteps[18] = ConstantParameters::THREE_STEP_CELL_DISTANCE; //above top left
	surroundingCellSteps[19] = ConstantParameters::THREE_STEP_CELL_DISTANCE; //below top left
	surroundingCellSteps[20] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //above right
	surroundingCellSteps[21] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //below right
	surroundingCellSteps[22] = ConstantParameters::THREE_STEP_CELL_DISTANCE; //above bottom right
	surroundingCellSteps[23] = ConstantParameters::THREE_STEP_CELL_DISTANCE; //below bottom right
	surroundingCellSteps[24] = ConstantParameters::THREE_STEP_CELL_DISTANCE; //above top right
	surroundingCellSteps[25] = ConstantParameters::THREE_STEP_CELL_DISTANCE; //below top right
	
	for (unsigned int index = 0; index < ConstantParameters::TOTAL_BLOCKS_IN_ZONE; index++) 
	{
		totalSurroundingLife = 0.f;
		averageLife = 0.f;

		x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
		y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
		z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;

		//get surrounding indices
		if ( z < ( ConstantParameters::BLOCKS_Z_AXIS - 1 ) ) 
		{
			surroundingCellIndices[NEIGHBOR_UP_3D] = index + ConstantParameters::WIDTH_TIMES_HEIGHT; //above
		} 
		else 
		{
			surroundingCellIndices[NEIGHBOR_UP_3D] = index - ( ConstantParameters::WIDTH_TIMES_HEIGHT * ( ConstantParameters::BLOCKS_Z_AXIS - 1 ) ); //above
		}

		if ( z > 0 ) 
		{
			surroundingCellIndices[NEIGHBOR_DOWN_3D] = index - ConstantParameters::WIDTH_TIMES_HEIGHT; //below
		} 
		else 
		{
			surroundingCellIndices[NEIGHBOR_DOWN_3D] = index + ( ConstantParameters::WIDTH_TIMES_HEIGHT * ( ConstantParameters::BLOCKS_Z_AXIS - 1 ) ); //below
		}

		if ( x > 0 ) 
		{
			surroundingCellIndices[NEIGHBOR_WEST_3D] = index - 1; //left
			surroundingCellIndices[NEIGHBOR_UP_WEST_3D] = surroundingCellIndices[NEIGHBOR_UP_3D] - 1; //above left
			surroundingCellIndices[NEIGHBOR_DOWN_WEST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_3D] - 1; //below left
		} 
		else 
		{
			surroundingCellIndices[NEIGHBOR_WEST_3D] = index + ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //left
			surroundingCellIndices[NEIGHBOR_UP_WEST_3D] = surroundingCellIndices[NEIGHBOR_UP_3D] + ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //above left
			surroundingCellIndices[NEIGHBOR_DOWN_WEST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_3D] + ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //below left
		}

		if ( y > 0 ) 
		{
			surroundingCellIndices[NEIGHBOR_SOUTH_3D] = index - ConstantParameters::BLOCKS_Y_AXIS; //bottom
			surroundingCellIndices[NEIGHBOR_UP_SOUTH_3D] = surroundingCellIndices[NEIGHBOR_UP_3D] - ConstantParameters::BLOCKS_Y_AXIS; //above bottom
			surroundingCellIndices[NEIGHBOR_DOWN_SOUTH_3D] = surroundingCellIndices[NEIGHBOR_DOWN_3D] - ConstantParameters::BLOCKS_Y_AXIS; //below bottom
			surroundingCellIndices[NEIGHBOR_SOUTHWEST_3D] = surroundingCellIndices[NEIGHBOR_WEST_3D] - ConstantParameters::BLOCKS_Y_AXIS; //bottom left
			surroundingCellIndices[NEIGHBOR_UP_SOUTHWEST_3D] = surroundingCellIndices[NEIGHBOR_UP_WEST_3D] - ConstantParameters::BLOCKS_Y_AXIS; //above bottom left
			surroundingCellIndices[NEIGHBOR_DOWN_SOUTHWEST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_WEST_3D] - ConstantParameters::BLOCKS_Y_AXIS; //below bottom left
		} 
		else 
		{
			surroundingCellIndices[NEIGHBOR_SOUTH_3D] = index + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //bottom
			surroundingCellIndices[NEIGHBOR_UP_SOUTH_3D] = surroundingCellIndices[NEIGHBOR_UP_3D] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //above bottom
			surroundingCellIndices[NEIGHBOR_DOWN_SOUTH_3D] = surroundingCellIndices[NEIGHBOR_DOWN_3D] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //below bottom
			surroundingCellIndices[NEIGHBOR_SOUTHWEST_3D] = surroundingCellIndices[NEIGHBOR_WEST_3D] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //bottom left
			surroundingCellIndices[NEIGHBOR_UP_SOUTHWEST_3D] = surroundingCellIndices[NEIGHBOR_UP_WEST_3D] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //above bottom left
			surroundingCellIndices[NEIGHBOR_DOWN_SOUTHWEST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_WEST_3D] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //below bottom left
		}

		if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) 
		{
			surroundingCellIndices[NEIGHBOR_NORTH_3D] = index + ConstantParameters::BLOCKS_Y_AXIS; //top
			surroundingCellIndices[NEIGHBOR_UP_NORTH_3D] = surroundingCellIndices[NEIGHBOR_UP_3D] + ConstantParameters::BLOCKS_Y_AXIS; //above top
			surroundingCellIndices[NEIGHBOR_DOWN_NORTH_3D] = surroundingCellIndices[NEIGHBOR_DOWN_3D] + ConstantParameters::BLOCKS_Y_AXIS; //below top
			surroundingCellIndices[NEIGHBOR_NORTHWEST_3D] = surroundingCellIndices[NEIGHBOR_WEST_3D] + ConstantParameters::BLOCKS_Y_AXIS; //top left
			surroundingCellIndices[NEIGHBOR_UP_NORTHWEST_3D] = surroundingCellIndices[NEIGHBOR_UP_WEST_3D] + ConstantParameters::BLOCKS_Y_AXIS; //above top left
			surroundingCellIndices[NEIGHBOR_DOWN_NORTHWEST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_WEST_3D] + ConstantParameters::BLOCKS_Y_AXIS; //below top left
		} 
		else 
		{
			surroundingCellIndices[NEIGHBOR_NORTH_3D] = index - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //top
			surroundingCellIndices[NEIGHBOR_UP_NORTH_3D] = surroundingCellIndices[NEIGHBOR_UP_3D] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //above top
			surroundingCellIndices[NEIGHBOR_DOWN_NORTH_3D] = surroundingCellIndices[NEIGHBOR_DOWN_3D] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //below top
			surroundingCellIndices[NEIGHBOR_NORTHWEST_3D] = surroundingCellIndices[NEIGHBOR_WEST_3D] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //top left
			surroundingCellIndices[NEIGHBOR_UP_NORTHWEST_3D] = surroundingCellIndices[NEIGHBOR_UP_WEST_3D] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //above top left
			surroundingCellIndices[NEIGHBOR_DOWN_NORTHWEST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_WEST_3D] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //below top left
		}

		if ( x < ( ConstantParameters::BLOCKS_X_AXIS - 1 ) ) 
		{
			surroundingCellIndices[NEIGHBOR_EAST_3D] = index + 1; //right
			surroundingCellIndices[NEIGHBOR_UP_EAST_3D] = surroundingCellIndices[NEIGHBOR_UP_3D] + 1; //above right
			surroundingCellIndices[NEIGHBOR_DOWN_EAST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_3D] + 1; //below right
			surroundingCellIndices[NEIGHBOR_SOUTHEAST_3D] = surroundingCellIndices[NEIGHBOR_SOUTH_3D] + 1; //bottom right
			surroundingCellIndices[NEIGHBOR_UP_SOUTHEAST_3D] = surroundingCellIndices[NEIGHBOR_UP_SOUTH_3D] + 1; //above bottom right
			surroundingCellIndices[NEIGHBOR_DOWN_SOUTHEAST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_SOUTH_3D] + 1; //below bottom right
			surroundingCellIndices[NEIGHBOR_NORTHEAST_3D] = surroundingCellIndices[NEIGHBOR_NORTH_3D] + 1; //top right
			surroundingCellIndices[NEIGHBOR_UP_NORTHEAST_3D] = surroundingCellIndices[NEIGHBOR_UP_NORTH_3D] + 1; //above top right
			surroundingCellIndices[NEIGHBOR_DOWN_NORTHEAST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_NORTH_3D] + 1; //below top right
		} 
		else 
		{
			surroundingCellIndices[NEIGHBOR_EAST_3D] = index - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //right
			surroundingCellIndices[NEIGHBOR_UP_EAST_3D] = surroundingCellIndices[NEIGHBOR_UP_3D] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //above right
			surroundingCellIndices[NEIGHBOR_DOWN_EAST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_3D] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //below right
			surroundingCellIndices[NEIGHBOR_SOUTHEAST_3D] = surroundingCellIndices[NEIGHBOR_SOUTH_3D] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //bottom right
			surroundingCellIndices[NEIGHBOR_UP_SOUTHEAST_3D] = surroundingCellIndices[NEIGHBOR_UP_SOUTH_3D] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //above bottom right
			surroundingCellIndices[NEIGHBOR_DOWN_SOUTHEAST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_SOUTH_3D] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //below bottom right
			surroundingCellIndices[NEIGHBOR_NORTHEAST_3D] = surroundingCellIndices[NEIGHBOR_NORTH_3D] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //top right
			surroundingCellIndices[NEIGHBOR_UP_NORTHEAST_3D] = surroundingCellIndices[NEIGHBOR_UP_NORTH_3D] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //above top right
			surroundingCellIndices[NEIGHBOR_DOWN_NORTHEAST_3D] = surroundingCellIndices[NEIGHBOR_DOWN_NORTH_3D] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //below top right
		}

		//get total surrounding life and average
		for ( unsigned int surroundingIndex = 0; surroundingIndex < NUM_NEIGHBORS_3D; surroundingIndex++ ) 
		{
			unsigned int neighborIndex = surroundingCellIndices[surroundingIndex];
			float neighborLife = m_worldBlockCells[ neighborIndex ].m_lifeValue;
			float weightedNeighborLife = neighborLife* surroundingCellSteps[surroundingIndex];
			totalSurroundingLife += weightedNeighborLife;
		}
		averageLife = totalSurroundingLife * INVERSE_NUM_SURROUNDING_CELL_WEIGHTS;

		m_temporaryCellularVector[index].m_lifeValue = m_worldBlockCells[index].m_lifeValue;
		float lifeChange = 0.f;
		float maxBirthThreshold = m_currentGenome->GetMaxBirthThreshold();
		if ( averageLife < m_currentGenome->m_starveThreshold ) 
		{
			lifeChange = RangeMapFloat( averageLife, 0.f, m_currentGenome->m_starveThreshold, -1.f, 0.f );
		}
		else if ( averageLife > m_currentGenome->m_crowdThreshold )
		{
			lifeChange = RangeMapFloat( averageLife, m_currentGenome->m_crowdThreshold, 1.f, 0.f, -1.f );
		}
		else if ( averageLife > m_currentGenome->m_stableThreshold )
		{
			//No Change
		}
		else if ( averageLife < maxBirthThreshold )
		{
			lifeChange = RangeMapFloat( averageLife, m_currentGenome->m_starveThreshold, maxBirthThreshold, 0.f, 1.f );
		}
		else
		{
			lifeChange = RangeMapFloat( averageLife, maxBirthThreshold, m_currentGenome->m_stableThreshold, 1.f, 0.f );
		}

		m_temporaryCellularVector[index].m_lifeValue += lifeChange;
		m_temporaryCellularVector[index].m_lifeValue = ClampFloat(m_temporaryCellularVector[index].m_lifeValue, 0.f, 1.f);
	}

	for (unsigned int index = 0; index < ConstantParameters::TOTAL_BLOCKS_IN_ZONE; index++) 
	{
		m_worldBlockCells[index] = m_temporaryCellularVector[index];

		if ( m_worldBlockCells[index].m_lifeValue > 0.f ) 
		{
			x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
			y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
			z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;	
			m_renderer.AddCubeToBuffer(Vector3((float) x, (float) y, (float) z), m_worldBlockCells[index].m_lifeValue);
		}
	}

	m_renderer.PushCubeVerticesToVBO();
}