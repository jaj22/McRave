#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class StrategyTrackerClass
{
	map <UnitType, int> enemyComposition;
	map <UnitType, double> unitScore;
	double globalAllyStrength, globalEnemyStrength;
	int eZerg, eProtoss, eTerran;
	bool fastExpand = false;
	bool invis = false;
	bool rush = false;
	bool zealotWall = false;
public:
	double globalAlly() { return globalAllyStrength; }
	double globalEnemy() {	return globalEnemyStrength;	}
	map <UnitType, double>& getUnitScore() { return unitScore; }
	bool isFastExpand() { return fastExpand; }
	bool setFastExpand(bool FE) { fastExpand = FE; }
	bool needDetection() { return invis; }	
	bool isRush() { return rush; }
	bool needZealotWall() { return zealotWall; }
	int getNumberZerg() { return eZerg; }
	int getNumberProtoss() { return eProtoss; }
	int getNumberTerran() { return eTerran; }
	
	// Updating
	void update();
	void updateAlly();
	void updateEnemy();
	void updateComposition();
	void updateUnitScore(UnitType, int);	
};

typedef Singleton<StrategyTrackerClass> StrategyTracker;