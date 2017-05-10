#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"
#include "src\bwem.h"
#include "UnitInfo.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;
using namespace BWEM;

class UnitTrackerClass
{
	map <Unit, UnitInfoClass> enemyUnits;
	map <Unit, UnitInfoClass> allyUnits;
	map <UnitSizeType, int> allySizes;
	map <UnitSizeType, int> enemySizes;
	int supply; 

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
	void storeEnemyUnit(Unit, map<Unit, UnitInfoClass>&);
	void storeAllyUnit(Unit, map<Unit, UnitInfoClass>&);
	void decayUnit(Unit);
};

typedef Singleton<UnitTrackerClass> UnitTracker;