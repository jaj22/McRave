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
public:

	map <Unit, NexusInfo> getMyNexus() { return myNexus; }

	void update();
	void storeNexus();
	void removeNexus(Unit);
	void trainProbes();
	void updateDefenses();
};

typedef Singleton<NexusTrackerClass> NexusTracker;