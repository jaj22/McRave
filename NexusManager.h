#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"
#include "NexusInfo.h"

class NexusTrackerClass
{
	map <Unit, NexusInfo> myNexus;
public:
	void update();
	void storeNexus();
	void trainProbes();
};

typedef Singleton<NexusTrackerClass> NexusTracker;