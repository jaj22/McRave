#pragma once
#include <BWAPI.h>
#include "Singleton.h"
#include "UnitInfo.h"

using namespace BWAPI;
using namespace std;
using namespace UnitTypes;

class UnitTrackerClass
{
	map <Unit, UnitInfo> enemyUnits;
	map <Unit, UnitInfo> allyUnits;
	map <UnitSizeType, int> allySizes;
	map <UnitSizeType, int> enemySizes;
	int supply;
	int globalStrategy;
public:
	// Accessors
	map<Unit, UnitInfo>& getMyUnits() { return allyUnits; }
	map<Unit, UnitInfo>& getEnUnits() { return enemyUnits; }
	map<UnitSizeType, int>& getMySizes() { return allySizes; }
	map<UnitSizeType, int>& getEnSizes() { return enemySizes; }
	int getSupply() { return supply; }

	// Updating
	void update();
	void storeUnits();
	void removeUnits();
	void storeEnemyUnit(Unit);
	void storeAllyUnit(Unit);
	void decayUnit(Unit);

	void getLocalCalculation(Unit, Unit);
	void getGlobalCalculation(Unit, Unit);
	int getGlobalStrategy() { return globalStrategy; }
};

typedef Singleton<UnitTrackerClass> UnitTracker;