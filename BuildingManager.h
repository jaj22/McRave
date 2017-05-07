#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
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

	int getQueuedMineral() { return queuedMineral; }
	int getQueuedGas() { return queuedGas; }

	void update();
	void queueBuildings();
	void constructBuildings();
	void updateQueue(UnitType);
};

typedef Singleton<BuildingTrackerClass> BuildingTracker;