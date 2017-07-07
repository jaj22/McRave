#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class ProductionTrackerClass
{
	map <Unit, UnitType> idleLowProduction;
	map <Unit, UnitType> idleHighProduction;
	map <Unit, TechType> idleTech;
	map <Unit, UpgradeType> idleUpgrade;
	int reservedMineral, reservedGas;
	bool noZealots = false;
	bool gateSat = false;
	bool barracksSat = false;
	bool hatchSat = false;
public:
	map <Unit, UnitType>& getIdleLowProduction() { return idleLowProduction; }
	map <Unit, UnitType>& getIdleHighProduction() { return idleHighProduction; }
	map <Unit, TechType>& getIdleTech() { return idleTech; }
	map <Unit, UpgradeType>& getIdleUpgrade() { return idleUpgrade; }

	int getReservedMineral() { return reservedMineral; }
	int getReservedGas() { return reservedGas; }
	bool isGateSat() { return gateSat; }
	bool isBarracksSat() { return barracksSat; }

	void update();
	void updatePriorities();
	void updateProtoss();
	void updateTerran();
	void updateZerg();
	void updateReservedResources();
};

typedef Singleton<ProductionTrackerClass> ProductionTracker;