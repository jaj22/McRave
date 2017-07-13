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
	bool debugging = false;
public:
	void update();
	void drawAllyInfo();
	void drawEnemyInfo();
	void drawInformation();

	void startClock();
	void performanceTest(string);
	void sendText(string);
};

typedef Singleton<InterfaceTrackerClass> InterfaceTracker;