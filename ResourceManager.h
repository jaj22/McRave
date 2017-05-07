#pragma once
#include "ResourceInfo.h"
#include "Singleton.h"

class ResourceTrackerClass
{
	map <Unit, ResourceInfo> myMinerals;
	map <Unit, ResourceInfo> myGas;
	map <Unit, ResourceInfo> myBoulders;
	bool saturated;
public:

	map <Unit, ResourceInfo>& getMyMinerals() { return myMinerals; }
	map <Unit, ResourceInfo>& getMyGas() { return myGas; }
	map <Unit, ResourceInfo>& getMyBoulders() { return myBoulders; }

	bool isSaturated() { return saturated; }

	void storeMineral(Unit);
	void storeGas(Unit);
	void storeBoulder(Unit);
	void removeMineral(Unit);
	void removeGas(Unit);
	void update();
};

typedef Singleton<ResourceTrackerClass> ResourceTracker;