#pragma once
#include "Singleton.h"
#include <BWAPI.h>
#include <BWTA.h>

using namespace std;
using namespace BWAPI;
using namespace UnitTypes;

class StrategyTrackerClass
{
	map <UnitType, int> enemyComposition;
	map <UnitType, double> unitScore;
	double globalAllyStrength, globalEnemyStrength;
public:
	double globalAlly() { return globalAllyStrength; }
	double globalEnemy() {	return globalEnemyStrength;	}
	
	// Updating
	void update();
	void updateAlly();
	void updateEnemy();
	void updateComposition();
	void updateUnitScore(UnitType, int);
	
};

typedef Singleton<StrategyTrackerClass> StrategyTracker;