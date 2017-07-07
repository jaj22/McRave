#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class GridTrackerClass
{
	// Ally grids
	int aClusterGrid[1024][1024];
	int reserveGrid[256][256];
	int baseGrid[256][256];
	int pylonGrid[256][256];
	int batteryGrid[256][256];
	int bunkerGrid[256][256];
	int defenseGrid[256][256];

	// Enemy grids
	double eGroundGrid[1024][1024];
	double eAirGrid[1024][1024];
	double eGroundDistanceGrid[1024][1024];
	double eAirDistanceGrid[1024][1024];
	int eDetectorGrid[1024][1024];
	int eGroundClusterGrid[256][256];
	int eAirClusterGrid[256][256];
	int stasisClusterGrid[256][256];

	// Neutral grids	
	int resourceGrid[256][256];

	// Mobility grids
	int mobilityGrid[1024][1024];
	int antiMobilityGrid[1024][1024];
	int distanceGridHome[1024][1024];
	int reservePathHome[256][256];

	// Special Unit grids
	int observerGrid[1024][1024];
	int arbiterGrid[1024][1024];
	int templarGrid[256][256];

	// Other
	bool distanceAnalysis = false;
	bool mobilityAnalysis = false;
	Position armyCenter;
public:

	// Member functions
	void reset();
	void update();
	void updateAllyGrids();
	void updateEnemyGrids();
	void updateNeutralGrids();
	void updateMobilityGrids();
	void updateArbiterMovement(Unit);
	void updateObserverMovement(Unit);
	void updateAllyMovement(Unit, WalkPosition);
	void updateReservedLocation(UnitType, TilePosition);
	void updateGroundDistanceGrid();
	bool isAnalyzed() { return distanceAnalysis; }

	// Ally mini tile grid functions
	int getACluster(int x, int y) { return aClusterGrid[x][y]; }
	int getACluster(WalkPosition here) { return aClusterGrid[here.x][here.y]; }

	// Ally tile grid functions
	int getReserveGrid(int x, int y) { return reserveGrid[x][y]; }
	int getReserveGrid(TilePosition here) { return reserveGrid[here.x][here.y]; }
	int getBaseGrid(int x, int y) { return baseGrid[x][y]; }
	int getBaseGrid(TilePosition here) { return baseGrid[here.x][here.y]; }
	int getPylonGrid(int x, int y) { return pylonGrid[x][y]; }
	int getPylonGrid(TilePosition here) { return pylonGrid[here.x][here.y]; }
	int getBatteryGrid(int x, int y) { return batteryGrid[x][y]; }
	int getBatteryGrid(TilePosition here) { return batteryGrid[here.x][here.y]; }
	int getBunkerGrid(int x, int y) { return bunkerGrid[x][y]; }
	int getBunkerGrid(TilePosition here) { return bunkerGrid[here.x][here.y]; }
	int getDefenseGrid(int x, int y) { return defenseGrid[x][y]; }
	int getDefenseGrid(TilePosition here) { return defenseGrid[here.x][here.y]; }

	// Enemy mini tile grid functions
	double getEGroundGrid(int x, int y) { return eGroundGrid[x][y]; }
	double getEGroundGrid(WalkPosition here) { return eGroundGrid[here.x][here.y]; }
	double getEAirGrid(int x, int y) { return eAirGrid[x][y]; }
	double getEAirGrid(WalkPosition here) { return eAirGrid[here.x][here.y]; }
	double getEGroundDistanceGrid(int x, int y) { return eGroundDistanceGrid[x][y]; }
	double getEGroundDistanceGrid(WalkPosition here) { return eGroundDistanceGrid[here.x][here.y]; }
	double getEAirDistanceGrid(int x, int y) { return eAirDistanceGrid[x][y]; }
	double getEAirDistanceGrid(WalkPosition here) { return eAirDistanceGrid[here.x][here.y]; }	
	int getEDetectorGrid(int x, int y) { return eDetectorGrid[x][y]; }
	int getEDetectorGrid(WalkPosition here) { return eDetectorGrid[here.x][here.y]; }

	// Enemy tile grid functions
	int getEGroundCluster(int x, int y) { return eGroundClusterGrid[x][y]; }
	int getEGroundCluster(TilePosition here) { return eGroundClusterGrid[here.x][here.y]; }
	int getEAirCluster(int x, int y) { return eAirClusterGrid[x][y]; }
	int getEAirCluster(TilePosition here) { return eAirClusterGrid[here.x][here.y]; }
	int getStasisCluster(int x, int y) { return stasisClusterGrid[x][y]; }
	int getStasisCluster(TilePosition here) { return stasisClusterGrid[here.x][here.y]; }

	// Neutral functions
	int getResourceGrid(int x, int y) { return resourceGrid[x][y]; }
	int getResourceGrid(TilePosition here) { return resourceGrid[here.x][here.y]; }

	// Mobility functions
	int getMobilityGrid(int x, int y) { return mobilityGrid[x][y]; }
	int getMobilityGrid(WalkPosition here) { return mobilityGrid[here.x][here.y]; }
	int getAntiMobilityGrid(int x, int y) { return antiMobilityGrid[x][y]; }
	int getAntiMobilityGrid(WalkPosition here) { return antiMobilityGrid[here.x][here.y]; }
	int getDistanceHome(int x, int y) { return distanceGridHome[x][y]; }
	int getDistanceHome(WalkPosition here) { return distanceGridHome[here.x][here.y]; }

	// Special unit functions
	int getObserverGrid(int x, int y) { return observerGrid[x][y]; }
	int getObserverGrid(WalkPosition here) { return observerGrid[here.x][here.y]; }
	int getArbiterGrid(int x, int y) { return arbiterGrid[x][y]; }
	int getArbiterGrid(WalkPosition here) { return arbiterGrid[here.x][here.y]; }

	// Other functions
	Position getArmyCenter(){ return armyCenter; }
};
typedef Singleton<GridTrackerClass> GridTracker;


