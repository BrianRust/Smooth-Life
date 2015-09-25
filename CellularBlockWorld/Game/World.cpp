#include "World.hpp"
#include "cmath"

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
void World::BuildZone2D() {
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int z = 0;

	//m_solidBlocks.reserve(ConstantParameters::TOTAL_BLOCKS_IN_ZONE);
	//m_temporaryCellularVector.reserve(ConstantParameters::TOTAL_BLOCKS_IN_ZONE);

	int counter = 0;

	for (unsigned int index = 0; index < ConstantParameters::WIDTH_TIMES_HEIGHT; index++) {
		if ( (rand() % 100) < m_currentGenome->m_initialLifePercentageInWorld ) {
			x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
			y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
			z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;

			m_worldBlockCells[index].m_lifeValue = 1.f;
			m_renderer.AddCubeToBuffer(Vector3((float) x, (float) y, (float) z), 1.f);

			counter++;
		}
		else {
			m_worldBlockCells[index].m_lifeValue = 0.f;
		}

		m_temporaryCellularVector.push_back((CubeCell(0.f)));
	}

	m_renderer.PushCubeVerticesToVBO();
}

//----------------------------------------------------
void World::BuildZone3D() {
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int z = 0;

	//m_solidBlocks.clear();
	//m_temporaryCellularVector.clear();

	//m_solidBlocks.reserve(ConstantParameters::TOTAL_BLOCKS_IN_ZONE);
	m_temporaryCellularVector.reserve(ConstantParameters::TOTAL_BLOCKS_IN_ZONE);

	int counter = 0;

	for (unsigned int index = 0; index < ConstantParameters::TOTAL_BLOCKS_IN_ZONE; index++) {
		if ( (rand() % 100) < m_currentGenome->m_initialLifePercentageInWorld ) {
			x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
			y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
			z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;

			m_worldBlockCells[index].m_lifeValue = 1.f;
			m_renderer.AddCubeToBuffer(Vector3((float) x, (float) y, (float) z), 1.f);
			counter++;
		}
		else {
			m_worldBlockCells[index].m_lifeValue = 0.f;
		}

		m_temporaryCellularVector.push_back((CubeCell(0.f)));
	}

	m_renderer.PushCubeVerticesToVBO();
}

//----------------------------------------------------
bool World::ProcessKeyDownEvent( unsigned char virtualKeyCode) {
	m_isKeyDown[ virtualKeyCode ] = true;
	return true;
}

//----------------------------------------------------
bool World::ProcessKeyUpEvent( unsigned char virtualKeyCode) {
	m_isKeyDown[ virtualKeyCode ] = false;
	return true;
}

//----------------------------------------------------
void World::UpdateCameraFromInput( float deltaSeconds ) {
	m_camera.m_cameraPitch -= deltaSeconds * m_mousePositionZDifference * ConstantParameters::MOUSE_SPEED;
	m_camera.m_cameraYaw += deltaSeconds * m_mousePositionXDifference * ConstantParameters::MOUSE_SPEED;
}

//----------------------------------------------------
void World::FindCameraFacingVector() {
	m_camera.m_cameraFacingVector.x = cos(m_camera.m_cameraYaw) * cos(m_camera.m_cameraPitch);
	m_camera.m_cameraFacingVector.y = sin(m_camera.m_cameraYaw) * cos(m_camera.m_cameraPitch);
	m_camera.m_cameraFacingVector.z = -sin(m_camera.m_cameraPitch);
}

//----------------------------------------------------
void World::UpdatePlayerFromInput( float deltaSeconds ) {	
	if ( m_isKeyDown[ 'P' ] && !m_keyIsHeld ) {
		m_keyIsHeld = true;
		m_isPaused = !m_isPaused;
	}
	
	if ( m_isKeyDown[ 'T' ] && !m_keyIsHeld ) {
		m_keyIsHeld = true;
		m_renderer.DeleteBuffers();
		if (m_is2DWorld) {
			GameOfLifeCellularAutomataPass2D();
		} else {
			GameOfLifeCellularAutomataPass3D();
		}
	}

	if ( m_isKeyDown[ 'R' ] && !m_keyIsHeld ) {
		m_keyIsHeld = true;
		m_renderer.DeleteBuffers();
		BuildZone3D();
		m_is2DWorld = false;
	}

	if ( m_isKeyDown[ 'E' ] ) {
		m_renderer.DeleteBuffers();
		BuildZone2D();
		m_is2DWorld = true;
	}
	
	if (m_isKeyDown[ VK_LEFT ]) {
		m_camera.m_cameraYaw += deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ VK_RIGHT ]) {
		m_camera.m_cameraYaw -= deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ VK_UP ]) {
		m_camera.m_cameraPitch -= deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ VK_DOWN ]) {
		m_camera.m_cameraPitch += deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ 'W' ]) {
		m_camera.m_cameraPosition.x += cos(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
		m_camera.m_cameraPosition.y += sin(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ 'D' ]) {
		m_camera.m_cameraPosition.y -= cos(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
		m_camera.m_cameraPosition.x += sin(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ 'S' ]) {
		m_camera.m_cameraPosition.x -= cos(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
		m_camera.m_cameraPosition.y -= sin(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ 'A' ]) {
		m_camera.m_cameraPosition.y += cos(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
		m_camera.m_cameraPosition.x -= sin(m_camera.m_cameraYaw) * deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[ VK_SPACE ]) {
		m_camera.m_cameraPosition.z += deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if (m_isKeyDown[VK_CONTROL]) {
		m_camera.m_cameraPosition.z -= deltaSeconds * ConstantParameters::CAMERA_SPEED;
	}

	if ( !( m_isKeyDown[ 'T' ] || m_isKeyDown[ 'R' ] || m_isKeyDown[ 'P' ]) ) {
		m_keyIsHeld = false;
	}
}

//----------------------------------------------------
void World::CheckForGimbleLock() {
	if (m_camera.m_cameraPitch >= ConstantParameters::CAMERA_Z_MAX) {
		m_camera.m_cameraPitch = ConstantParameters::CAMERA_Z_MAX - 0.005f;
	}

	if (m_camera.m_cameraPitch <= -ConstantParameters::CAMERA_Z_MAX) {
		m_camera.m_cameraPitch = -ConstantParameters::CAMERA_Z_MAX + 0.005f;
	}
}

//----------------------------------------------------
void World::Update() {
	float currentTime = (float) Time::GetCurrentTimeSeconds();
	float deltaSeconds = ConstantParameters::DELTA_SECONDS; // Hack: assume 60 FPS

	UpdatePlayerFromInput( deltaSeconds );
	UpdateCameraFromInput( deltaSeconds );
	FindCameraFacingVector();
	CheckForGimbleLock();

	if (!m_isPaused) {
		if (m_is2DWorld) {
			m_renderer.DeleteBuffers();
			GameOfLifeCellularAutomataPass2D();
		} else {
			if ( ( currentTime - m_lastCellularAutomataTime ) > ConstantParameters::TIME_BETWEEN_CA_STEPS ) {
				m_lastCellularAutomataTime = currentTime;
				m_renderer.DeleteBuffers();
				GameOfLifeCellularAutomataPass3D();
			}
		}
	}
}

//----------------------------------------------------
void World::Render() {	
	m_renderer.SetModelViewProjectionMatrix(m_camera);
	m_renderer.SendCubeVBO();
	m_renderer.PopMatrix();
}

//----------------------------------------------------
void World::GameOfLifeCellularAutomataPass2D() {
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int z = 0;

	float TOTAL_SURROUNDING_LIFE = 0.f;
	float LIFE_AVERAGE = 0.f;
	float NUM_SURROUNDING_CELLS = 0.f;
	
	for (unsigned int index = 0; index < ConstantParameters::WIDTH_TIMES_HEIGHT; index++) {
		x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
		y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
		z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;

		if ( x > 0 ) {	//left
			TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index - 1 ].m_lifeValue * ConstantParameters::ONE_STEP_CELL_DISTANCE ); //left
			NUM_SURROUNDING_CELLS++;

			if ( y > 0 ) {
				TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index - ConstantParameters::BLOCKS_Y_AXIS ].m_lifeValue * ConstantParameters::ONE_STEP_CELL_DISTANCE ); //bottom
				TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index - 1 - ConstantParameters::BLOCKS_Y_AXIS ].m_lifeValue * ConstantParameters::TWO_STEP_CELL_DISTANCE ); //left bottom
				NUM_SURROUNDING_CELLS++;
				NUM_SURROUNDING_CELLS++;
			}

			if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) {
				TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + ConstantParameters::BLOCKS_Y_AXIS ].m_lifeValue * ConstantParameters::ONE_STEP_CELL_DISTANCE ); //top
				TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index - 1 + ConstantParameters::BLOCKS_Y_AXIS ].m_lifeValue * ConstantParameters::TWO_STEP_CELL_DISTANCE ); //left top
				NUM_SURROUNDING_CELLS++;
				NUM_SURROUNDING_CELLS++;
			}
		}

		if ( x < ( ConstantParameters::BLOCKS_X_AXIS - 1 ) ) {	//right
			TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + 1 ].m_lifeValue * ConstantParameters::ONE_STEP_CELL_DISTANCE ); //right
			NUM_SURROUNDING_CELLS++;
			
			if ( y > 0 ) {	//bottom right
				TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + 1 - ConstantParameters::BLOCKS_Y_AXIS ].m_lifeValue * ConstantParameters::TWO_STEP_CELL_DISTANCE ); //bottom right
				NUM_SURROUNDING_CELLS++;
			}

			if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) {	//above right
				TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + 1 + ConstantParameters::BLOCKS_Y_AXIS ].m_lifeValue * ConstantParameters::TWO_STEP_CELL_DISTANCE ); //above right
				NUM_SURROUNDING_CELLS++;
			}
		}

		LIFE_AVERAGE = TOTAL_SURROUNDING_LIFE / NUM_SURROUNDING_CELLS;

		if ( ( LIFE_AVERAGE > m_currentGenome->m_starveThreshold ) && ( LIFE_AVERAGE < m_currentGenome->m_stableThreshold) ) { //gain life
			float lifeGainedOffset = ( 1.f - ( std::abs( ( LIFE_AVERAGE / m_currentGenome->m_maxBirthThreshold ) - 1.f ) ) );
			float lifeGained = m_currentGenome->m_maxBirthsPerStep * lifeGainedOffset;
			m_temporaryCellularVector[index].m_lifeValue = ( m_worldBlockCells[index].m_lifeValue + lifeGained );
		} else if ( LIFE_AVERAGE < m_currentGenome->m_starveThreshold ) {
			float lifeLostOffset = ( 1.f - ( LIFE_AVERAGE / m_currentGenome->m_starveThreshold ) );
			float lifeLost = m_currentGenome->m_maxStarveLossPerStep * lifeLostOffset;
			m_temporaryCellularVector[index].m_lifeValue = ( m_worldBlockCells[index].m_lifeValue - lifeLost );
		} else if ( LIFE_AVERAGE > m_currentGenome->m_crowdThreshold ) {
			float lifeLostOffset = ( ( LIFE_AVERAGE / m_currentGenome->m_crowdThreshold ) - 1.f );
			float lifeLost = m_currentGenome->m_maxCrowdLossPerStep * lifeLostOffset;
			m_temporaryCellularVector[index].m_lifeValue = ( m_worldBlockCells[index].m_lifeValue - lifeLost );
		} else {
			m_temporaryCellularVector[index].m_lifeValue = m_worldBlockCells[index].m_lifeValue;
		}

		TOTAL_SURROUNDING_LIFE = 0.f;
		NUM_SURROUNDING_CELLS = 0.f;
		LIFE_AVERAGE = 0.f;
	}

	for (unsigned int index = 0; index < ConstantParameters::WIDTH_TIMES_HEIGHT; index++) {
		m_worldBlockCells[index] = m_temporaryCellularVector[index];

		if ( m_worldBlockCells[index].m_lifeValue > 0.f ) {
			x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
			y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
			z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;	
			m_renderer.AddCubeToBuffer(Vector3((float) x, (float) y, (float) z), m_worldBlockCells[index].m_lifeValue);
		}
	}

	m_renderer.PushCubeVerticesToVBO();
}

//----------------------------------------------------
void World::GameOfLifeCellularAutomataPass3D() {
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int z = 0;

	float TOTAL_SURROUNDING_LIFE = 0.f;
	float LIFE_AVERAGE = 0.f;
	float NUM_SURROUNDING_CELLS = 0.f;
	
	for (unsigned int index = 0; index < ConstantParameters::TOTAL_BLOCKS_IN_ZONE; index++) {
		x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
		y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
		z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;

		//Above
		if ( z < ( ConstantParameters::BLOCKS_Z_AXIS - 1 ) ) {
			TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + ConstantParameters::WIDTH_TIMES_HEIGHT ].m_lifeValue * ConstantParameters::ONE_STEP_CELL_DISTANCE ); //above
			NUM_SURROUNDING_CELLS++;

			if ( x > 0 ) {	//above left
				TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + ConstantParameters::WIDTH_TIMES_HEIGHT - 1 ].m_lifeValue * ConstantParameters::TWO_STEP_CELL_DISTANCE ); //above left
				NUM_SURROUNDING_CELLS++;
				
				if ( y > 0 ) {
					TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + ConstantParameters::WIDTH_TIMES_HEIGHT - ConstantParameters::BLOCKS_Y_AXIS ].m_lifeValue * ConstantParameters::TWO_STEP_CELL_DISTANCE ); //above bottom
					NUM_SURROUNDING_CELLS++;
					
					TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + ConstantParameters::WIDTH_TIMES_HEIGHT - 1 - ConstantParameters::BLOCKS_Y_AXIS ].m_lifeValue * ConstantParameters::THREE_STEP_CELL_DISTANCE ); //above left bottom
					NUM_SURROUNDING_CELLS++;
				}

				if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) {
					TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + ConstantParameters::WIDTH_TIMES_HEIGHT + ConstantParameters::BLOCKS_Y_AXIS ].m_lifeValue * ConstantParameters::TWO_STEP_CELL_DISTANCE ); //above top
					NUM_SURROUNDING_CELLS++;

					TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + ConstantParameters::WIDTH_TIMES_HEIGHT - 1 + ConstantParameters::BLOCKS_Y_AXIS ].m_lifeValue * ConstantParameters::THREE_STEP_CELL_DISTANCE ); //above left top
					NUM_SURROUNDING_CELLS++;
				}
			}

			if ( x < ( ConstantParameters::BLOCKS_X_AXIS - 1 ) ) {	//above right
				TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + ConstantParameters::WIDTH_TIMES_HEIGHT + 1 ].m_lifeValue * ConstantParameters::TWO_STEP_CELL_DISTANCE ); //above right
				NUM_SURROUNDING_CELLS++;
				
				if ( y > 0 ) {	//above right bottom
					TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ index + ConstantParameters::WIDTH_TIMES_HEIGHT + 1 - ConstantParameters::BLOCKS_Y_AXIS ].m_lifeValue * ConstantParameters::THREE_STEP_CELL_DISTANCE ); //above right bottom
					NUM_SURROUNDING_CELLS++;
					
				}

				//where i was
				if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) {	//above right top
					if ( m_worldBlockCells[ index + ConstantParameters::WIDTH_TIMES_HEIGHT + 1 + ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid ) {
						SOLID_COUNTER++;
					}
				}
			}
		}

		//Below
		if ( z > 0 ) {
			if ( m_worldBlockCells[ index - ConstantParameters::WIDTH_TIMES_HEIGHT ].m_isSolid ) {	//below
				SOLID_COUNTER++;
			}

			if ( x > 0 ) {	//below left
				if ( m_worldBlockCells[ index - ConstantParameters::WIDTH_TIMES_HEIGHT - 1 ].m_isSolid ) {	//below left
					SOLID_COUNTER++;
				}

				if ( y > 0 ) {
					if ( m_worldBlockCells[ index - ConstantParameters::WIDTH_TIMES_HEIGHT - ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid ) {	//below bottom
						SOLID_COUNTER++;
					}

					if ( m_worldBlockCells[ index - ConstantParameters::WIDTH_TIMES_HEIGHT - 1 - ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid ) { //below left bottom
						SOLID_COUNTER++;
					}
				}

				if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) {
					if ( m_worldBlockCells[ index - ConstantParameters::WIDTH_TIMES_HEIGHT + ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid ) {	//below top
						SOLID_COUNTER++;
					}

					if ( m_worldBlockCells[ index - ConstantParameters::WIDTH_TIMES_HEIGHT - 1 + ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid ) { //below left top
						SOLID_COUNTER++;
					}
				}
			}

			if ( x < ( ConstantParameters::BLOCKS_X_AXIS - 1 ) ) {	//below right
				if ( m_worldBlockCells[ index - ConstantParameters::WIDTH_TIMES_HEIGHT + 1 ].m_isSolid ) {	//below right
					SOLID_COUNTER++;
				}

				if ( y > 0 ) {	//below right bottom
					if ( m_worldBlockCells[ index - ConstantParameters::WIDTH_TIMES_HEIGHT + 1 - ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid ) {
						SOLID_COUNTER++;
					}
				}

				if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) {	//below right top
					if ( m_worldBlockCells[ index - ConstantParameters::WIDTH_TIMES_HEIGHT + 1 + ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid ) {
						SOLID_COUNTER++;
					}
				}
			}
		}

		//Middle
		if ( x > 0 ) {	//left
			if ( m_worldBlockCells[ index - 1 ].m_isSolid ) {	//left
				SOLID_COUNTER++;
			}

			if ( y > 0 ) {
				if ( m_worldBlockCells[ index - ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid ) {	//bottom
					SOLID_COUNTER++;
				}

				if ( m_worldBlockCells[ index - 1 - ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid ) {	//left bottom
					SOLID_COUNTER++;
				}
			}

			if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) {
				if ( m_worldBlockCells[ index + ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid ) { //top
					SOLID_COUNTER++;
				}

				if ( m_worldBlockCells[ index - 1 + ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid ) {	//left top
					SOLID_COUNTER++;
				}
			}
		}

		if ( x < ( ConstantParameters::BLOCKS_X_AXIS - 1 ) ) {	//right
			if ( m_worldBlockCells[ index + 1 ].m_isSolid ) {	//right
				SOLID_COUNTER++;
			}

			if ( y > 0 ) {	//bottom right
				if ( m_worldBlockCells[ index + 1 - ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid )
					SOLID_COUNTER++;
			}

			if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) {	//above right
				if ( m_worldBlockCells[ index + 1 + ConstantParameters::BLOCKS_Y_AXIS ].m_isSolid )
					SOLID_COUNTER++;
			}
		}

		LIFE_AVERAGE = TOTAL_SURROUNDING_LIFE / NUM_SURROUNDING_CELLS;

		if ( ( LIFE_AVERAGE > m_currentGenome->m_starveThreshold ) && ( LIFE_AVERAGE < m_currentGenome->m_stableThreshold) ) { //gain life
			float lifeGainedOffset = ( 1.f - ( std::abs( ( LIFE_AVERAGE / m_currentGenome->m_maxBirthThreshold ) - 1.f ) ) );
			float lifeGained = m_currentGenome->m_maxBirthsPerStep * lifeGainedOffset;
			m_temporaryCellularVector[index].m_lifeValue = ( m_worldBlockCells[index].m_lifeValue + lifeGained );
		} else if ( LIFE_AVERAGE < m_currentGenome->m_starveThreshold ) {
			float lifeLostOffset = ( 1.f - ( LIFE_AVERAGE / m_currentGenome->m_starveThreshold ) );
			float lifeLost = m_currentGenome->m_maxStarveLossPerStep * lifeLostOffset;
			m_temporaryCellularVector[index].m_lifeValue = ( m_worldBlockCells[index].m_lifeValue - lifeLost );
		} else if ( LIFE_AVERAGE > m_currentGenome->m_crowdThreshold ) {
			float lifeLostOffset = ( ( LIFE_AVERAGE / m_currentGenome->m_crowdThreshold ) - 1.f );
			float lifeLost = m_currentGenome->m_maxCrowdLossPerStep * lifeLostOffset;
			m_temporaryCellularVector[index].m_lifeValue = ( m_worldBlockCells[index].m_lifeValue - lifeLost );
		} else {
			m_temporaryCellularVector[index].m_lifeValue = m_worldBlockCells[index].m_lifeValue;
		}

		TOTAL_SURROUNDING_LIFE = 0.f;
		NUM_SURROUNDING_CELLS = 0.f;
		LIFE_AVERAGE = 0.f;
	}

	for (unsigned int index = 0; index < ConstantParameters::TOTAL_BLOCKS_IN_ZONE; index++) {
		m_worldBlockCells[index] = m_temporaryCellularVector[index];

		if ( m_worldBlockCells[index].m_lifeValue > 0.f ) {
			x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
			y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
			z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;	
			m_renderer.AddCubeToBuffer(Vector3((float) x, (float) y, (float) z), m_worldBlockCells[index].m_lifeValue);
		}
	}

	m_renderer.PushCubeVerticesToVBO();
}