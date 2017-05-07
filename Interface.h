#pragma once
#include "Singleton.h"
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class InterfaceTrackerClass
{
public:
	void update();
};

typedef Singleton<InterfaceTrackerClass> InterfaceTracker;