#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"
#include "BuildingInfo.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;
using namespace UnitTypes;

class BuildingTrackerClass
{
	int queuedMineral, queuedGas;
	map <UnitType, pair<TilePosition, Unit>> queuedBuildings;
	map <Unit, BuildingInfoClass> myBuildings;
public:
	TilePosition getBuildLocation(UnitType);
	TilePosition getCannonLocation();
	TilePosition getBuildLocationNear(UnitType, TilePosition, bool);
	int getQueuedMineral() { return queuedMineral; }
	int getQueuedGas() { return queuedGas; }
	map <UnitType, pair<TilePosition, Unit>>& getQueuedBuildings() { return queuedBuildings; }
	map <Unit, BuildingInfoClass> getMyBuildings() { return myBuildings; }

	void update();
	void queueBuildings();
	void constructBuildings();
	void updateQueue(Unit);
	void storeBuilding(Unit);
	void removeBuilding(Unit);
};

typedef Singleton<BuildingTrackerClass> BuildingTracker;