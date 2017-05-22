#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"
#include "src\bwem.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class GridTrackerClass
{
	// Unit grids
	double enemyGroundStrengthGrid[256][256];
	double enemyAirStrengthGrid[256][256];
	int enemyGroundClusterGrid[256][256];
	int enemyAirClusterGrid[256][256];
	int tankClusterGrid[256][256];
	int allyClusterGrid[256][256];

	// Enemy mini grid testing
	double enemyGroundStrengthMiniGrid[1024][1024];

	// Neutral grids	
	int resourceGrid[256][256];

	// Mobility grids
	int mobilityMiniGrid[1024][1024];
	int antiMobilityMiniGrid[1024][1024];
	int mobilityGrid[256][256];

	// Special Unit grids
	int observerGrid[256][256];
	int arbiterGrid[256][256];
	int templarGrid[256][256];

	Position supportPosition;
	Position earlyDefensePosition;
public:
	void reset();
	void update();	
	void updateAllyGrids();
	void updateEnemyGrids();
	void updateNeutralGrids();
	void updateMobilityGrids();
	void updateObserverGrids();
	void updateArbiterGrids();
	void updateAllyMovement(Unit, WalkPosition);

	Position getSupportPosition() { return supportPosition; }
	Position getEarlyDefensePosition() { return earlyDefensePosition; }
	double getEnemyGrd(int x, int y) { return enemyGroundStrengthGrid[x][y]; }

	double getEnemyMiniGrd(int x, int y) { return enemyGroundStrengthMiniGrid[x][y]; }


	double getEnemyAir(int x, int y) { return enemyAirStrengthGrid[x][y]; }	
	int getEnemyGrdCluster(int x, int y) { return enemyGroundClusterGrid[x][y]; }
	int getEnemyAirCluster(int x, int y) { return enemyAirClusterGrid[x][y]; }
	int getTankCluster(int x, int y) { return tankClusterGrid[x][y]; }
	int getAllyCluster(int x, int y) { return allyClusterGrid[x][y]; }
	
	int getResourceGrid(int x, int y) { return resourceGrid[x][y]; }
	int getMobilityGrid(int x, int y) { return mobilityGrid[x][y]; }
	int getMobilityMiniGrid(int x, int y) { return mobilityMiniGrid[x][y]; }
	int getAntiMobilityMiniGrid(int x, int y) { return antiMobilityMiniGrid[x][y]; }

	int getObserverGrid(int x, int y) { return observerGrid[x][y]; }
	int getArbiterGrid(int x, int y) { return arbiterGrid[x][y]; }
};

typedef Singleton<GridTrackerClass> GridTracker;