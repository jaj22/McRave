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
	map<Unit, UnitInfoClass>& getMyUnits() { return allyUnits; }
	map<Unit, UnitInfoClass>& getEnUnits() { return enemyUnits; }
	map<UnitSizeType, int>& getMySizes() { return allySizes; }
	map<UnitSizeType, int>& getEnSizes() { return enemySizes; }
	int getSupply() { return supply; }

	void update();

	// Command manager
	void unitMicroTarget(Unit, Unit);
	void unitExploreArea(Unit);
	void unitGetCommand(Unit);
	void commandUpdate();

	// Strategy manager
	int unitGetGlobalStrategy();
	void unitGetLocalStrategy(Unit, Unit);

	// Target manager
	void unitGetTarget(Unit);
	void unitGetClusterTarget(Unit);	

	// Special units
	void templarManager(Unit);
	void reaverManager(Unit);
	void arbiterManager(Unit);

	void storeEnemyUnit(Unit, map<Unit, UnitInfoClass>&);
	void storeAllyUnit(Unit, map<Unit, UnitInfoClass>&);
	void removeUnit(Unit);

	Position unitFlee(Unit unit, Unit currentTarget);
};

typedef Singleton<UnitTrackerClass> UnitTracker;