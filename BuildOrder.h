#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class BuildOrderTrackerClass
{
	map <UnitType, int> buildingDesired;
	int earlyBuild, midBuild, lateBuild;
	bool getEarlyBuild = true, getMidBuild = false, getLateBuild = false;
public:
	map <UnitType, int>& getBuildingDesired() { return buildingDesired; }
	
	bool isEarlyBuild() { return getEarlyBuild; }
	bool isMidBuild() { return getMidBuild; }
	bool isLateBuild() { return getLateBuild; }

	void update();
	void earlyBuilds();
	void midBuilds();
	void lateBuilds();
};

typedef Singleton<BuildOrderTrackerClass> BuildOrderTracker;