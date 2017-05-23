#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class BuildingTrackerClass
{
	int queuedMineral, queuedGas;
	map <UnitType, pair<TilePosition, Unit>> queuedBuildings;
public:	
	TilePosition getBuildLocation(UnitType);
	TilePosition getCannonLocation();
	TilePosition getNexusLocation();
	TilePosition getGasLocation();

	TilePosition getBuildLocationNear(UnitType, TilePosition, bool);

	// unused for now
	TilePosition getProductionLocationNear(UnitType, TilePosition, bool);
	TilePosition getCannonLocationNear(UnitType, TilePosition, bool);

	int getQueuedMineral() { return queuedMineral; }
	int getQueuedGas() { return queuedGas; }
	map <UnitType, pair<TilePosition, Unit>>& getQueuedBuildings() { return queuedBuildings; }

	void update();
	void queueBuildings();
	void constructBuildings();
	void updateQueue(Unit);
};

typedef Singleton<BuildingTrackerClass> BuildingTracker;