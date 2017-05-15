#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class ProductionTrackerClass
{
	map <int, UnitType> idleBuildings;
	map <int, UnitType> idleGates;
	map <int, TechType> idleTech;
	map <int, UpgradeType> idleUpgrade;
	int reservedMineral, reservedGas;
	bool noZealots = false;
public:
	map <int, UnitType>& getIdleBuildings() { return idleBuildings; }
	map <int, UnitType>& getIdleGates() { return idleGates; }
	map <int, TechType>& getIdleTech() { return idleTech; }
	map <int, UpgradeType>& getIdleUpgrade() { return idleUpgrade; }

	int getReservedMineral() { return reservedMineral; }
	int getReservedGas() { return reservedGas; }
	bool getNoZealots() { return noZealots; }

	void update();
	void updateGateway(Unit);
	void updateRobo(Unit);
	void updateStargate(Unit);
	void updateLuxuryTech(Unit);
	void updateRequiredTech(Unit);
	void updateReservedResources();
	void setNoZealots(bool);
};

typedef Singleton<ProductionTrackerClass> ProductionTracker;