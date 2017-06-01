#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class UnitUtilClass
{
	bool stimResearched;
public:
	double getStrength(UnitType);
	double getAirStrength(UnitType);
	double getVisibleStrength(Unit);
	double getTrueRange(UnitType, Player);
	double getTrueAirRange(UnitType, Player);
	double getPriority(UnitType);
	double getTrueSpeed(UnitType, Player);
	WalkPosition getMiniTile(Unit);
	set<WalkPosition> getMiniTilesUnderUnit(Unit);
};

typedef Singleton<UnitUtilClass> UnitUtil;