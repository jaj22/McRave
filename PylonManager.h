#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;
using namespace UnitTypes;

class PylonTrackerClass
{
	map<TilePosition, int> smallLocations;
	map<TilePosition, int> mediumLocations;
	map<TilePosition, int> largeLocations;
	set<Unit> myPylons;
public:
	void update();
	void addToGrid(Unit);
	bool hasPower(TilePosition, UnitType);
};

typedef Singleton<PylonTrackerClass> PylonTracker;