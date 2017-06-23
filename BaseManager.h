#pragma once
#include <BWAPI.h>
#include "Singleton.h"
#include "BaseInfo.h"

using namespace BWAPI;
using namespace std;

class BaseTrackerClass
{	
	map <Unit, BaseInfo> myBases;
public:
	map <Unit, BaseInfo>& getMyBases() { return myBases; }

	void update();
	void storeBase(Unit);
	void removeBase(Unit);
	void trainWorkers(BaseInfo&);
	void updateDefenses(BaseInfo&);
	TilePosition centerOfResources(Unit);
};

typedef Singleton<BaseTrackerClass> BaseTracker;