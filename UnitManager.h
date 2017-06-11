#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"
#include "src\bwem.h"
#include "UnitInfo.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;
using namespace UnitTypes;
using namespace BWEM;

class UnitTrackerClass
{
	map <Unit, UnitInfoClass> enemyUnits;
	map <Unit, UnitInfoClass> allyUnits;
	map <UnitSizeType, int> allySizes;
	map <UnitSizeType, int> enemySizes;
	int supply; 
	int globalStrategy;
public:
	// Accessors
	map<Unit, UnitInfoClass>& getMyUnits() { return allyUnits; }
	map<Unit, UnitInfoClass>& getEnUnits() { return enemyUnits; }
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