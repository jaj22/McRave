#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

// Credits to Andrew Smith, the author of Skynet, for this pylon power checker
// Broodwar has serious issues with figuring out if a building can be powered at a location
// This pylon tracker helps by hardcoding positions around a pylon that are suitable locations

class PylonTrackerClass
{
	map<TilePosition, int> smallPowerSites;
	map<TilePosition, int> mediumPowerSites;
	map<TilePosition, int> largePowerSites;
public:
	void update();
	void addToGrid(Unit);
	bool hasPower(TilePosition, UnitType);
};

typedef Singleton<PylonTrackerClass> PylonTracker;