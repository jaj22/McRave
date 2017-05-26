#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class CommandTrackerClass
{
	int globalStrategy;
public:
	void update();
	void getDecision(Unit, Unit);
	void getLocalCalculation(Unit, Unit);
	void getGlobalCalculation(Unit, Unit);

	void unitGetLocalStrategy(Unit, Unit);
	int unitGetGlobalStrategy();
	void microTarget(Unit, Unit);
	void exploreArea(Unit);
	void fleeTarget(Unit, Unit);
};

typedef Singleton<CommandTrackerClass> CommandTracker;