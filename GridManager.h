#pragma once
#include "Singleton.h"
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class GridTrackerClass
{
	double enemyGroundStrengthGrid[256][256];
	double enemyAirStrengthGrid[256][256];
	int enemyGroundClusterGrid[256][256];
	int enemyAirClusterGrid[256][256];
	int tankClusterGrid[256][256];
	int allyClusterGrid[256][256];
	int allyDetectorGrid[256][256];
	int resourceGrid[256][256];
public:
	void reset();
	void update();	
	void updateAllyGrids();
	void updateEnemyGrids();
	void updateNeutralGrids();

	double getEGroundGrid(int x, int y) { return enemyGroundStrengthGrid[x][y]; }
	double getEAairGrid(int x, int y) { return enemyAirStrengthGrid[x][y]; }	
	int getEGroundCluster(int x, int y) { return enemyGroundClusterGrid[x][y]; }
	int getEAirCluster(int x, int y) { return enemyAirClusterGrid[x][y]; }
	int getTankCluster(int x, int y) { return tankClusterGrid[x][y]; }
	int getACluster(int x, int y) { return allyClusterGrid[x][y]; }
	int getADetectorGrid(int x, int y) { return allyDetectorGrid[x][y]; }
	int getResourceGrid(int x, int y) { return resourceGrid[x][y]; }
};

typedef Singleton<GridTrackerClass> GridTracker;