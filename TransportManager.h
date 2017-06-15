#pragma once
#include <BWAPI.h>
#include "Singleton.h"
#include "SpecialUnitInfo.h"

using namespace BWAPI;
using namespace std;
using namespace UnitTypes;

class TransportTrackerClass
{
	map <Unit, TransportInfo> myShuttles;
public:
	void update();
	void updateCargo(TransportInfo&);
	void updateDecision(TransportInfo&);
	void updateMovement(TransportInfo&);
	void removeUnit(Unit);
	void storeUnit(Unit);
};

typedef Singleton<TransportTrackerClass> TransportTracker;