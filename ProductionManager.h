#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class ProductionTrackerClass
{
	map <int, UnitType> idleBuildings;
	map <int, UnitType> idleGates;
	map <int, TechType> idleTech;
	map <int, UpgradeType> idleUpgrade;
	int reservedMineral, reservedGas;
public:
	map <int, UnitType>& getIdleBuildings() { return idleBuildings; }
	map <int, UnitType>& getIdleGates() { return idleGates; }
	map <int, TechType>& getIdleTech() { return idleTech; }
	map <int, UpgradeType>& getIdleUpgrade() { return idleUpgrade; }

	int getReservedMineral() { return reservedMineral; }
	int getReservedGas() { return reservedGas; }

	void update();
	void updateGateway();
	void updateReservedResources();
};

typedef Singleton<ProductionTrackerClass> ProductionTracker;