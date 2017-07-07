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
	double allyDefense, enemyDefense;
	int eZerg, eProtoss, eTerran;
	bool fastExpand = false;
	bool invis = false;
	bool rush = false;
	bool holdRamp = false;
	bool walledOff = false;
	bool bust = false;
public:
	double globalAlly() { return globalAllyStrength; }
	double globalEnemy() {	return globalEnemyStrength;	}
	double getAllyDefense() { return allyDefense; }
	double getEnemyDefense() { return enemyDefense; }
	map <UnitType, double>& getUnitScore() { return unitScore; }
	bool isFastExpand() { return fastExpand; }
	bool setFastExpand(bool FE) { fastExpand = FE; }
	bool needDetection() { return invis; }	
	bool isRush() { return rush; }
	bool isHoldRamp() { return holdRamp; }
	bool isWalled() { return walledOff; }
	bool isBust() { return bust; }
	int getNumberZerg() { return eZerg; }
	int getNumberProtoss() { return eProtoss; }
	int getNumberTerran() { return eTerran; }
	
	// Updating
	void update();
	void updateAlly();
	void updateEnemy();
	void updateSituationalBehaviour();
	void updateUnitScore(UnitType, int);	

	void protossStrategy();
	void terranStrategy();
	void zergStrategy();

	void increaseGlobalAlly(int increase) { globalAllyStrength += increase; }
	void increaseGlobalEnemy(int increase) { globalEnemyStrength += increase; }
};

typedef Singleton<StrategyTrackerClass> StrategyTracker;