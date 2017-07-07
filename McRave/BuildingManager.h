#pragma once
#include <BWAPI.h>
#include "Singleton.h"
#include "BuildingInfo.h"

using namespace BWAPI;
using namespace std;

class BuildingTrackerClass
{
	int queuedMineral, queuedGas;
	map <UnitType, int> buildingsQueued;
	map <Unit, BuildingInfo> myBuildings;
	map <Unit, BuildingInfo> myBatteries;
	int errorTime = 0;
public:
	map <Unit, BuildingInfo>& getMyBuildings() { return myBuildings; }
	map <Unit, BuildingInfo>& getMyBatteries() { return myBatteries; }
	map <UnitType, int>& getbuildingsQueued() { return buildingsQueued; }

	int getQueuedMineral() { return queuedMineral; }
	int getQueuedGas() { return queuedGas; }
	TilePosition getBuildLocation(UnitType);
	TilePosition getBuildLocationNear(UnitType, TilePosition, bool);
	bool canBuildHere(UnitType, TilePosition, bool ignoreCond = false);

	void update();
	void queueBuildings();
	void constructBuildings();
	void storeBuilding(Unit);
	void storeBattery(Unit);
	void removeBuilding(Unit);
};

typedef Singleton<BuildingTrackerClass> BuildingTracker;