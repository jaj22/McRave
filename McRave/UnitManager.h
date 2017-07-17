#pragma once
#include <BWAPI.h>
#include "Singleton.h"
#include "UnitInfo.h"

using namespace BWAPI;
using namespace std;

class UnitTrackerClass
{
	map <Unit, UnitInfo> enemyUnits;
	map <Unit, UnitInfo> allyUnits;
	map <UnitSizeType, int> allySizes;
	map <UnitSizeType, int> enemySizes;

	// Testing stuff
	set <Bullet> myBullets;
	map <UnitType, double> unitPerformance;

	int supply, globalStrategy;
public:
	// Accessors
	map<Unit, UnitInfo>& getMyUnits() { return allyUnits; }
	map<Unit, UnitInfo>& getEnUnits() { return enemyUnits; }
	map<UnitSizeType, int>& getMySizes() { return allySizes; }
	map<UnitSizeType, int>& getEnSizes() { return enemySizes; }

	// Updating
	void update();
	void updateAliveUnits();
	void updateDeadUnits();
	void updateEnemy(UnitInfo&);
	void updateAlly(UnitInfo&);	
	void getLocalCalculation(UnitInfo&);
	void updateGlobalCalculations();

	// One shot storage
	void onUnitCreate(Unit);
	void onUnitComplete(Unit);
	void onUnitMorph(Unit);

	void storeAlly(Unit);
	void storeEnemy(Unit);
	void removeUnit(Unit);

	// Returns the global strategy
	int getGlobalStrategy() { return globalStrategy; }

	// Returns the non BWAPI based supply count
	int getSupply() { return supply; }
};

typedef Singleton<UnitTrackerClass> UnitTracker;