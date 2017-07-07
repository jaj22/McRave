#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class InterfaceTrackerClass
{
public:
	void update();
	void drawAllyInfo(Unit);
	void drawEnemyInfo(Unit);
};

typedef Singleton<InterfaceTrackerClass> InterfaceTracker;