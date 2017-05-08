#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class TargetTrackerClass
{
public:
	void update();
	void unitGetTarget(Unit);
	void unitGetClusterTarget(Unit);
};

typedef Singleton<TargetTrackerClass> TargetTracker;