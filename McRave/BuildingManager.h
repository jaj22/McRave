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
	map <Unit, BuildingInfo> myBunkers;
	int errorTime = 0, buildingOffset = 0;
public:
	map <Unit, BuildingInfo>& getMyBuildings() { return myBuildings; }
	map <Unit, BuildingInfo>& getMyBatteries() { return myBatteries; }
	map <Unit, BuildingInfo>& getMyBunkers() { return myBunkers; }
	map <UnitType, int>& getbuildingsQueued() { return buildingsQueued; }
	TilePosition getBuildLocation(UnitType);
	TilePosition getBuildLocationNear(UnitType, TilePosition, bool);
	bool canBuildHere(UnitType, TilePosition, bool ignoreCond = false);

	// Returns the minerals that are reserved for queued buildings
	int getQueuedMineral() { return queuedMineral; }

	// Returns the gas that is reserved for queued buildings
	int getQueuedGas() { return queuedGas; }

	void update();
	void queueBuildings();
	void constructBuildings();
	void storeBuilding(Unit);
	void storeBattery(Unit);
	void removeBuilding(Unit);
};

typedef Singleton<BuildingTrackerClass> BuildingTracker;