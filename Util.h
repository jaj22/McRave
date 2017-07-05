#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class UtilTrackerClass
{
	bool stimResearched;
public:
	double getStrength(UnitType, Player);
	double getAirStrength(UnitType, Player);
	double getVisibleStrength(Unit, Player);
	double getTrueRange(UnitType, Player);
	double getTrueAirRange(UnitType, Player);
	double getTrueGroundDamage(UnitType, Player);
	double getTrueAirDamage(UnitType, Player);
	double getPriority(UnitType, Player);
	double getTrueSpeed(UnitType, Player);
	int getMinStopFrame(UnitType);
	WalkPosition getWalkPosition(Unit);
	set<WalkPosition> getWalkPositionsUnderUnit(Unit);
};

typedef Singleton<UtilTrackerClass> UtilTracker;