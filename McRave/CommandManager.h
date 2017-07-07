#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class CommandTrackerClass
{
public:
	void update();
	void getDecision(UnitInfo&);
	void defend(UnitInfo&);
	void attackMove(UnitInfo&);	
	void microTarget(UnitInfo&);
	void exploreArea(UnitInfo&);
	void fleeTarget(UnitInfo&);
};

typedef Singleton<CommandTrackerClass> CommandTracker;