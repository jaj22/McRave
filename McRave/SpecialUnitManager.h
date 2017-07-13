#pragma once
#include <BWAPI.h>
#include "SpecialUnitInfo.h"

using namespace BWAPI;
using namespace std;

class SpecialUnitTrackerClass
{
	map <Unit, SupportUnitInfo> myArbiters;
	map <Unit, SupportUnitInfo> myObservers;
	map <Unit, SupportUnitInfo> myTemplars;
	map <Unit, SupportUnitInfo> myReavers;
	map <Unit, SupportUnitInfo> myMedics;
public:
	void update();
	void updateArbiters();
	void updateObservers();
	void updateReavers();
	void storeUnit(Unit);
	void removeUnit(Unit);

	map <Unit, SupportUnitInfo>& getMyArbiters() { return myArbiters; }
	map <Unit, SupportUnitInfo>& getMyObservers() { return myObservers; }
	map <Unit, SupportUnitInfo>& getMyTemplars() { return myTemplars; }
	map <Unit, SupportUnitInfo>& getMyReavers() { return myReavers; }
};

typedef Singleton<SpecialUnitTrackerClass> SpecialUnitTracker;