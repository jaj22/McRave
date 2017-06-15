#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class CommandTrackerClass
{
public:
	void update();
	void getDecision(Unit, Unit);
	void defend(Unit, Unit);
	void attackMove(Unit, Unit);

	void unitGetLocalStrategy(Unit, Unit);
	void microTarget(Unit, Unit);
	void exploreArea(Unit);
	void fleeTarget(Unit, Unit);
};

typedef Singleton<CommandTrackerClass> CommandTracker;