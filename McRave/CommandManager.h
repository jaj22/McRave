#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class CommandTrackerClass
{
	bool kite, approach;
public:
	void update();
	void updateAlliedUnits();
	void defend(UnitInfo&);
	void attackMove(UnitInfo&);	
	void attackTarget(UnitInfo&);
	void exploreArea(UnitInfo&);
	void fleeTarget(UnitInfo&);
	void approachTarget(UnitInfo&);
};

typedef Singleton<CommandTrackerClass> CommandTracker;