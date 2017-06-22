#pragma once
#include <BWAPI.h>
#include "Singleton.h"
#include "BuildingInfo.h"

using namespace BWAPI;
using namespace std;

class BuildingTrackerClass
{
	int queuedMineral, queuedGas;
	map <UnitType, pair<TilePosition, Unit>> queuedBuildings;
	map <Unit, BuildingInfo> myBuildings;
	map <Unit, BuildingInfo> myBatteries;
public:
	TilePosition getBuildLocation(UnitType);
	TilePosition getCannonLocation();
	TilePosition getBuildLocationNear(UnitType, TilePosition, bool);
	int getQueuedMineral() { return queuedMineral; }
	int getQueuedGas() { return queuedGas; }
	map <UnitType, pair<TilePosition, Unit>>& getQueuedBuildings() { return queuedBuildings; }
	map <Unit, BuildingInfo> getMyBuildings() { return myBuildings; }

	void update();
	void queueBuildings();
	void constructBuildings();
	void updateQueue(Unit);
	void storeBuilding(Unit);
	void removeBuilding(Unit);
};

typedef Singleton<BuildingTrackerClass> BuildingTracker;