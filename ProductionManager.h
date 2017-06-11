#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;
using namespace UnitTypes;

class ProductionTrackerClass
{
	map <Unit, UnitType> idleBuildings;
	map <Unit, UnitType> idleGates;
	map <Unit, TechType> idleTech;
	map <Unit, UpgradeType> idleUpgrade;
	int reservedMineral, reservedGas;
	bool noZealots = false;
public:
	map <Unit, UnitType>& getIdleBuildings() { return idleBuildings; }
	map <Unit, UnitType>& getIdleGates() { return idleGates; }
	map <Unit, TechType>& getIdleTech() { return idleTech; }
	map <Unit, UpgradeType>& getIdleUpgrade() { return idleUpgrade; }

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