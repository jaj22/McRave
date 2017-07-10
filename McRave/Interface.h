#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class InterfaceTrackerClass
{
	clock_t globalClock;
	int screenOffset = 0;
	map <string, clock_t> myTest;
public:
	void update();
	void drawAllyInfo(Unit);
	void drawEnemyInfo(Unit);

	void startClock();
	void performanceTest(string);
};

typedef Singleton<InterfaceTrackerClass> InterfaceTracker;