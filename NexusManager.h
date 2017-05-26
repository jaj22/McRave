#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"
#include "NexusInfo.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class NexusTrackerClass
{
	map <Unit, NexusInfo> myNexus;
	map <UnitType, pair<TilePosition, Unit>> queuedCannons;
public:

	map <Unit, NexusInfo>& getMyNexus() { return myNexus; }

	void update();
	void storeNexus();
	void removeNexus(Unit);
	void trainProbes();
	void updateDefenses();

	void createCannonPosition();
	void createPylonPosition();
};

typedef Singleton<NexusTrackerClass> NexusTracker;