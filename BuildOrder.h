#pragma once
#include <BWAPI.h>
#include <vector>
#include "Singleton.h"

using namespace std;
using namespace BWAPI;

class BuildOrderTrackerClass
{
	map <UnitType, int> buildingDesired;
	int earlyBuild, midBuild, lateBuild;
public:
	map <UnitType, int>& getBuildingDesired() { return buildingDesired; }

	void update();
	void earlyBuilds();
	void midBuilds();
	void lateBuilds();
};

typedef Singleton<BuildOrderTrackerClass> BuildOrderTracker;