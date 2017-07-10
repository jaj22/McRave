#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class UtilTrackerClass
{
	bool stimResearched;
public:
	double getStrength(UnitInfo&, Player);
	double getAirStrength(UnitInfo&, Player);
	double getVisibleStrength(UnitInfo&, Player);
	double getPriority(UnitInfo&, Player);

	double getTrueRange(UnitType, Player);
	double getTrueAirRange(UnitType, Player);
	double getTrueGroundDamage(UnitType, Player);
	double getTrueAirDamage(UnitType, Player);
	double getTrueSpeed(UnitType, Player);
	int getMinStopFrame(UnitType);
	WalkPosition getWalkPosition(Unit);
	set<WalkPosition> getWalkPositionsUnderUnit(Unit);

	// Check if a location is safe based on what type of checks are desired and the size of the unit.
	bool isSafe(WalkPosition start, WalkPosition finish, UnitType, bool groundCheck, bool airCheck, bool mobilityCheck);
	
};

typedef Singleton<UtilTrackerClass> UtilTracker;