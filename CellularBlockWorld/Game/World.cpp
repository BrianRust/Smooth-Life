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
	m_genomesForTesting.push_back(Genome(0.05f, 0.05f, 0.05f, 0.1875f, 0.4375f, 0.3125f, ConstantParameters::SOLID_BLOCK_PERCENTAGE_2D));
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
	const unsigned int NUM_SURROUNDING_CELLS_INT = 8;
	const float INVERSE_NUM_SURROUNDING_CELLS = 1.f/8.f;

	unsigned int surroundingCellIndices[8];
	float surroundingCellSteps[8];

	//set feathering values
	surroundingCellSteps[0] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //left
	surroundingCellSteps[1] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //bottom
	surroundingCellSteps[2] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //bottom left
	surroundingCellSteps[3] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //top
	surroundingCellSteps[4] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //top left
	surroundingCellSteps[5] = ConstantParameters::ONE_STEP_CELL_DISTANCE; //right
	surroundingCellSteps[6] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //bottom right
	surroundingCellSteps[7] = ConstantParameters::TWO_STEP_CELL_DISTANCE; //top right
	
	for (unsigned int index = 0; index < ConstantParameters::WIDTH_TIMES_HEIGHT; index++) {
		x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
		y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
		z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;

		//get surrounding indices
		if ( x > 0 ) {
			surroundingCellIndices[0] = index - 1; //left
		} else {
			surroundingCellIndices[0] = ( index + ConstantParameters::BLOCKS_X_AXIS ) - 1; //left
		}

		if ( y > 0 ) {
			surroundingCellIndices[1] = index - ConstantParameters::BLOCKS_Y_AXIS; //bottom
			surroundingCellIndices[2] = surroundingCellIndices[0] - ConstantParameters::BLOCKS_Y_AXIS; //bottom left
		} else {
			surroundingCellIndices[1] = index + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //bottom
			surroundingCellIndices[2] = surroundingCellIndices[0] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //bottom left
		}

		if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) {
			surroundingCellIndices[3] = index + ConstantParameters::BLOCKS_Y_AXIS; //top
			surroundingCellIndices[4] = surroundingCellIndices[0] + ConstantParameters::BLOCKS_Y_AXIS; //top left
		} else {
			surroundingCellIndices[3] = index - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //top
			surroundingCellIndices[4] = surroundingCellIndices[0] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //top left
		}

		if ( x < ( ConstantParameters::BLOCKS_X_AXIS - 1 ) ) {
			surroundingCellIndices[5] = index + 1; //right
			surroundingCellIndices[6] = surroundingCellIndices[1] + 1; //bottom right
			surroundingCellIndices[7] = surroundingCellIndices[3] + 1; //top right
		} else {
			surroundingCellIndices[5] = index - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //right
			surroundingCellIndices[6] = surroundingCellIndices[1] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //bottom right
			surroundingCellIndices[7] = surroundingCellIndices[3] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //top right
		}

		//get total surrounding life and average
		for ( unsigned int surroundingIndex = 0; surroundingIndex < NUM_SURROUNDING_CELLS_INT; surroundingIndex++ ) {
			TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ surroundingCellIndices[surroundingIndex] ].m_lifeValue * surroundingCellSteps[surroundingIndex] );
		}
		LIFE_AVERAGE = TOTAL_SURROUNDING_LIFE * INVERSE_NUM_SURROUNDING_CELLS;

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
		//NUM_SURROUNDING_CELLS = 0.f;
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

	const unsigned int NUM_SURROUNDING_CELLS_INT = 26;
	const float INVERSE_NUM_SURROUNDING_CELLS = 1.f/26.f;

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
	
	for (unsigned int index = 0; index < ConstantParameters::TOTAL_BLOCKS_IN_ZONE; index++) {
		x = index & ConstantParameters::BLOCKS_X_AXIS - 1;
		y = (index >> ConstantParameters::BLOCKS_Y_POWER) & ConstantParameters::BLOCKS_Y_AXIS - 1;
		z = (index >> ConstantParameters::BLOCKS_XY_POWER) & ConstantParameters::BLOCKS_Z_AXIS - 1;

		//get surrounding indices
		if ( z < ( ConstantParameters::BLOCKS_Z_AXIS - 1 ) ) {
			surroundingCellIndices[8] = index + ConstantParameters::WIDTH_TIMES_HEIGHT; //above
		} else {
			surroundingCellIndices[8] = index - ( ConstantParameters::WIDTH_TIMES_HEIGHT * ( ConstantParameters::BLOCKS_Z_AXIS - 1 ) ); //above
		}

		if ( z > 0 ) {
			surroundingCellIndices[9] = index - ConstantParameters::WIDTH_TIMES_HEIGHT; //below
		} else {
			surroundingCellIndices[9] = index + ( ConstantParameters::WIDTH_TIMES_HEIGHT * ( ConstantParameters::BLOCKS_Z_AXIS - 1 ) ); //below
		}

		if ( x > 0 ) {
			surroundingCellIndices[0] = index - 1; //left
			surroundingCellIndices[10] = surroundingCellIndices[8] - 1; //above left
			surroundingCellIndices[11] = surroundingCellIndices[9] - 1; //below left
		} else {
			surroundingCellIndices[0] = index + ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //left
			surroundingCellIndices[10] = surroundingCellIndices[8] + ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //above left
			surroundingCellIndices[11] = surroundingCellIndices[9] + ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //below left
		}

		if ( y > 0 ) {
			surroundingCellIndices[1] = index - ConstantParameters::BLOCKS_Y_AXIS; //bottom
			surroundingCellIndices[12] = surroundingCellIndices[8] - ConstantParameters::BLOCKS_Y_AXIS; //above bottom
			surroundingCellIndices[13] = surroundingCellIndices[9] - ConstantParameters::BLOCKS_Y_AXIS; //below bottom
			surroundingCellIndices[2] = surroundingCellIndices[0] - ConstantParameters::BLOCKS_Y_AXIS; //bottom left
			surroundingCellIndices[14] = surroundingCellIndices[10] - ConstantParameters::BLOCKS_Y_AXIS; //above bottom left
			surroundingCellIndices[15] = surroundingCellIndices[11] - ConstantParameters::BLOCKS_Y_AXIS; //below bottom left
		} else {
			surroundingCellIndices[1] = index + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //bottom
			surroundingCellIndices[12] = surroundingCellIndices[8] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //above bottom
			surroundingCellIndices[13] = surroundingCellIndices[9] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //below bottom
			surroundingCellIndices[2] = surroundingCellIndices[0] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //bottom left
			surroundingCellIndices[14] = surroundingCellIndices[10] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //above bottom left
			surroundingCellIndices[15] = surroundingCellIndices[11] + ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //below bottom left
		}

		if ( y < ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) ) {
			surroundingCellIndices[3] = index + ConstantParameters::BLOCKS_Y_AXIS; //top
			surroundingCellIndices[16] = surroundingCellIndices[8] + ConstantParameters::BLOCKS_Y_AXIS; //above top
			surroundingCellIndices[17] = surroundingCellIndices[9] + ConstantParameters::BLOCKS_Y_AXIS; //below top
			surroundingCellIndices[4] = surroundingCellIndices[0] + ConstantParameters::BLOCKS_Y_AXIS; //top left
			surroundingCellIndices[18] = surroundingCellIndices[10] + ConstantParameters::BLOCKS_Y_AXIS; //above top left
			surroundingCellIndices[19] = surroundingCellIndices[11] + ConstantParameters::BLOCKS_Y_AXIS; //below top left
		} else {
			surroundingCellIndices[3] = index - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //top
			surroundingCellIndices[16] = surroundingCellIndices[8] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //above top
			surroundingCellIndices[17] = surroundingCellIndices[9] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //below top
			surroundingCellIndices[4] = surroundingCellIndices[0] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //top left
			surroundingCellIndices[18] = surroundingCellIndices[10] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //above top left
			surroundingCellIndices[19] = surroundingCellIndices[11] - ( ( ConstantParameters::BLOCKS_Y_AXIS - 1 ) * ConstantParameters::BLOCKS_X_AXIS ); //below top left
		}

		if ( x < ( ConstantParameters::BLOCKS_X_AXIS - 1 ) ) {
			surroundingCellIndices[5] = index + 1; //right
			surroundingCellIndices[20] = surroundingCellIndices[8] + 1; //above right
			surroundingCellIndices[21] = surroundingCellIndices[9] + 1; //below right
			surroundingCellIndices[6] = surroundingCellIndices[1] + 1; //bottom right
			surroundingCellIndices[22] = surroundingCellIndices[12] + 1; //above bottom right
			surroundingCellIndices[23] = surroundingCellIndices[13] + 1; //below bottom right
			surroundingCellIndices[7] = surroundingCellIndices[3] + 1; //top right
			surroundingCellIndices[24] = surroundingCellIndices[16] + 1; //above top right
			surroundingCellIndices[25] = surroundingCellIndices[17] + 1; //below top right
		} else {
			surroundingCellIndices[5] = index - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //right
			surroundingCellIndices[20] = surroundingCellIndices[8] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //above right
			surroundingCellIndices[21] = surroundingCellIndices[9] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //below right
			surroundingCellIndices[6] = surroundingCellIndices[1] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //bottom right
			surroundingCellIndices[22] = surroundingCellIndices[12] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //above bottom right
			surroundingCellIndices[23] = surroundingCellIndices[13] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //below bottom right
			surroundingCellIndices[7] = surroundingCellIndices[3] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //top right
			surroundingCellIndices[24] = surroundingCellIndices[16] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //above top right
			surroundingCellIndices[25] = surroundingCellIndices[17] - ( ConstantParameters::BLOCKS_X_AXIS - 1 ); //below top right
		}

		//get total surrounding life and average
		for ( unsigned int surroundingIndex = 0; surroundingIndex < NUM_SURROUNDING_CELLS_INT; surroundingIndex++ ) {
			TOTAL_SURROUNDING_LIFE += ( m_worldBlockCells[ surroundingCellIndices[surroundingIndex] ].m_lifeValue * surroundingCellSteps[surroundingIndex] );
		}
		LIFE_AVERAGE = TOTAL_SURROUNDING_LIFE * INVERSE_NUM_SURROUNDING_CELLS;

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