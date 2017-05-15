#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"
#include "SpecialUnitInfo.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class SpecialUnitTrackerClass
{
	map <Unit, SpecialUnitInfoClass> myArbiters;
	map <Unit, SpecialUnitInfoClass> myObservers;
	map <Unit, SpecialUnitInfoClass> myTemplars;
public:

	void update();
	void updateArbiters();
	void updateObservers();
	void updateTemplars();
	void storeUnits();
	void removeUnits();

	map <Unit, SpecialUnitInfoClass>& getMyArbiters() { return myArbiters; }
	map <Unit, SpecialUnitInfoClass>& getMyObservers() { return myObservers; }
	map <Unit, SpecialUnitInfoClass>& getMyTemplars() { return myTemplars; }
};

typedef Singleton<SpecialUnitTrackerClass> SpecialUnitTracker;