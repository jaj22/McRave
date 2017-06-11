#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;
using namespace UnitTypes;

class InterfaceTrackerClass
{
public:
	void update();
};

typedef Singleton<InterfaceTrackerClass> InterfaceTracker;