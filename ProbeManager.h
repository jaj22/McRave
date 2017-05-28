#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"
#include "ProbeInfo.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class ProbeTrackerClass
{
	map <Unit, ProbeInfo> myProbes;
	Unit scout;
	bool scouting = true;
public:

	bool isScouting() { return scouting; }
	map <Unit, ProbeInfo>& getMyProbes() { return myProbes; }

	void update();
	void storeProbe(Unit);
	void removeProbe(Unit);
	void assignProbe(Unit);	
	void reAssignProbe(Unit);
	void scoutProbe();
	void enforceAssignments();
};

typedef Singleton<ProbeTrackerClass> ProbeTracker;