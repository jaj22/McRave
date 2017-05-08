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
	void update();
	void storeNexus();
	void trainProbes();
};

typedef Singleton<NexusTrackerClass> NexusTracker;