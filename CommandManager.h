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
	void updateDecisions(Unit, Unit);
	void updateLocalStrategy(Unit, Unit);
	void updateGlobalStrategy(Unit, Unit);


	void unitGetLocalStrategy(Unit, Unit);
	int unitGetGlobalStrategy();
	void unitMicroTarget(Unit, Unit);
	void unitExploreArea(Unit);
	Position unitFlee(Unit, Unit);
};

typedef Singleton<CommandTrackerClass> CommandTracker;