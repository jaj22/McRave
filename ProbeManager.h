#pragma once
#include <BWAPI.h>
#include "Singleton.h"
#include "ProbeInfo.h"

using namespace BWAPI;
using namespace std;

class ProbeTrackerClass
{
	map <Unit, ProbeInfo> myProbes;
	map <WalkPosition, int> recentExplorations;
	Unit scout;
	bool scouting = true;
public:

	bool isScouting() { return scouting; }
	map <Unit, ProbeInfo>& getMyProbes() { return myProbes; }
	Unit getScout() { return scout; }
	
	void update();
	void updateScout(ProbeInfo&);
	void updateDecision(ProbeInfo&);

	void storeProbe(Unit);
	void removeProbe(Unit);
	void assignProbe(Unit);	
	void reAssignProbe(Unit);	
	void exploreArea(Unit);
	void avoidEnemy(Unit);

	Unit getClosestProbe(Position);
};

typedef Singleton<ProbeTrackerClass> ProbeTracker;