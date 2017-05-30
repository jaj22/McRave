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
	// Ally grids	
	int aClusterGrid[1024][1024];	
	int reserveGrid[256][256];
	int nexusGrid[256][256];

	// Enemy grids
	double eGroundGrid[1024][1024];
	double eAirGrid[1024][1024];
	double eDistanceGrid[1024][1024];
	int eGroundClusterGrid[256][256];
	int eAirClusterGrid[256][256];
	int stasisClusterGrid[256][256];

	// Neutral grids	
	int resourceGrid[256][256];		

	// Mobility grids
	int mobilityGrid[1024][1024];
	int antiMobilityGrid[1024][1024];
	int distanceGridHome[1024][1024];

	// Special Unit grids
	int observerGrid[1024][1024];
	int arbiterGrid[256][256];
	int templarGrid[256][256];

	bool distanceOnce = true;
public:
	
	// Member functions
	void reset();
	void update();	
	void updateAllyGrids();
	void updateEnemyGrids();
	void updateNeutralGrids();
	void updateMobilityGrids();
	void updateArbiterGrids();
	void updateObserverMovement(Unit);
	void updateAllyMovement(Unit, WalkPosition);
	void updateReservedLocation(UnitType, TilePosition);
	void updateDistanceGrid();

	// Ally functions
	int getACluster(int x, int y) { return aClusterGrid[x][y]; }
	int getResourceGrid(int x, int y) { return resourceGrid[x][y]; }
	int getNexusGrid(int x, int y) { return nexusGrid[x][y]; }

	// Enemy functions
	double getEGroundGrid(int x, int y) { return eGroundGrid[x][y]; }
	double getEAirGrid(int x, int y) { return eAirGrid[x][y]; }
	double getEDistanceGrid(int x, int y) { return eDistanceGrid[x][y]; }
	int getEGroundCluster(int x, int y) { return eGroundClusterGrid[x][y]; }
	int getEAirCluster(int x, int y) { return eAirClusterGrid[x][y]; }
	int getStasisCluster(int x, int y) { return stasisClusterGrid[x][y]; }	
	
	// Neutral functions
	int getReserveGrid(int x, int y) { return reserveGrid[x][y]; }
	
	// Mobility functions
	int getMobilityGrid(int x, int y) { return mobilityGrid[x][y]; }
	int getAntiMobilityGrid(int x, int y) { return antiMobilityGrid[x][y]; }
	int getDistanceHome(int x, int y) { return distanceGridHome[x][y]; }	

	// Special unit functions
	int getObserverGrid(int x, int y) { return observerGrid[x][y]; }
	int getArbiterGrid(int x, int y) { return arbiterGrid[x][y]; }
};

typedef Singleton<GridTrackerClass> GridTracker;