#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class CommandTrackerClass
{
	bool kite;
public:
	void update();
	void updateAlliedUnits();
	void defend(UnitInfo&);
	void attackMove(UnitInfo&);	
	void attackTarget(UnitInfo&);
	void exploreArea(UnitInfo&);
	void fleeTarget(UnitInfo&);
};

typedef Singleton<CommandTrackerClass> CommandTracker;