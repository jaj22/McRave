#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class TargetTrackerClass
{
public:
	Unit getTarget(UnitInfo&);
	Unit enemyTarget(UnitInfo&);
	Unit allyTarget(UnitInfo&);
};

typedef Singleton<TargetTrackerClass> TargetTracker;