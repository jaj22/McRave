#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class InterfaceTrackerClass
{
	clock_t globalClock = clock();
	int screenOffset = 0;
public:
	void update();
	void drawAllyInfo(Unit);
	void drawEnemyInfo(Unit);

	void performanceTest(string);
};

typedef Singleton<InterfaceTrackerClass> InterfaceTracker;