#pragma once
#include <BWAPI.h>
#include "Singleton.h"
#include "SpecialUnitInfo.h"

using namespace BWAPI;
using namespace std;

class TransportTrackerClass
{
	map <Unit, TransportInfo> myShuttles;
	map <WalkPosition, int> recentExplorations;
public:
	void update();
	void updateTransports();
	void updateCargo(TransportInfo&);
	void updateDecision(TransportInfo&);
	void updateMovement(TransportInfo&);
	void removeUnit(Unit);
	void storeUnit(Unit);
};

typedef Singleton<TransportTrackerClass> TransportTracker;