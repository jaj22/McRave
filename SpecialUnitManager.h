#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"
#include "SpecialUnitInfo.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;
using namespace UnitTypes;

class SpecialUnitTrackerClass
{
	map <Unit, SpecialUnitInfoClass> myArbiters;
	map <Unit, SpecialUnitInfoClass> myObservers;
	map <Unit, SpecialUnitInfoClass> myTemplars;
	map <Unit, SpecialUnitInfoClass> myShuttles;
	map <Unit, SpecialUnitInfoClass> myReavers;
public:
	void update();
	void updateArbiters();
	void updateObservers();
	void updateTemplars();
	void updateShuttles();
	void updateReavers();
	void storeUnit(Unit);
	void removeUnit(Unit);

	map <Unit, SpecialUnitInfoClass>& getMyArbiters() { return myArbiters; }
	map <Unit, SpecialUnitInfoClass>& getMyObservers() { return myObservers; }
	map <Unit, SpecialUnitInfoClass>& getMyTemplars() { return myTemplars; }
};

typedef Singleton<SpecialUnitTrackerClass> SpecialUnitTracker;