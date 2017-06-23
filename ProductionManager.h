#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class ProductionTrackerClass
{
	map <Unit, UnitType> idleBuildings;
	map <Unit, UnitType> idleGates;
	map <Unit, TechType> idleTech;
	map <Unit, UpgradeType> idleUpgrade;
	int reservedMineral, reservedGas;
	bool noZealots = false;
	bool gateSat = false;
public:
	map <Unit, UnitType>& getIdleBuildings() { return idleBuildings; }
	map <Unit, UnitType>& getIdleGates() { return idleGates; }
	map <Unit, TechType>& getIdleTech() { return idleTech; }
	map <Unit, UpgradeType>& getIdleUpgrade() { return idleUpgrade; }

	int getReservedMineral() { return reservedMineral; }
	int getReservedGas() { return reservedGas; }
	bool getNoZealots() { return noZealots; }
	bool isGateSat() { return gateSat; }

	void update();
	void updateProtoss();
	void updateTerran();
	void updateGateway(Unit);
	void updateRobo(Unit);
	void updateStargate(Unit);
	void updateLuxuryTech(Unit);
	void updateRequiredTech(Unit);
	void updateReservedResources();
	void setNoZealots(bool);
};

typedef Singleton<ProductionTrackerClass> ProductionTracker;