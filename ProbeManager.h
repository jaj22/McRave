#pragma once
#include <BWAPI.h>
#include "Singleton.h"
#include "ProbeInfo.h"

using namespace std;
using namespace BWAPI;

class ProbeTrackerClass
{
	map <Unit, ProbeInfo> myProbes;
	Unit scout;
public:
	void update();
	void storeProbe(Unit);
	void removeProbe(Unit);
	void assignProbe(Unit);	
};

typedef Singleton<ProbeTrackerClass> ProbeTracker;