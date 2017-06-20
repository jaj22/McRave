#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class TargetTrackerClass
{
public:
	Unit getTarget(Unit);
	Unit singleTarget(Unit);
	Unit clusterTarget(Unit);
};

typedef Singleton<TargetTrackerClass> TargetTracker;