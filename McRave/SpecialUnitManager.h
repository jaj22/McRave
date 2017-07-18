#pragma once
#include <BWAPI.h>
#include "SpecialUnitInfo.h"

using namespace BWAPI;
using namespace std;

class SpecialUnitTrackerClass
{
	map <Unit, SupportUnitInfo> myArbiters;
	map <Unit, SupportUnitInfo> myDarchons;
	map <Unit, SupportUnitInfo> myDetectors;
	map <Unit, SupportUnitInfo> myTemplars;
	map <Unit, SupportUnitInfo> myReavers;
	map <WalkPosition, double> myStorms;
	Unit recaller = nullptr;
public:
	void update();
	void updateArbiters();
	void updateDarchons();
	void updateDefilers();
	void updateDetectors();
	void updateQueens();
	void updateReavers();
	void storeUnit(Unit);
	void removeUnit(Unit);

	map <Unit, SupportUnitInfo>& getMyArbiters() { return myArbiters; }
	map <Unit, SupportUnitInfo>& getMyDetectors() { return myDetectors; }
	map <Unit, SupportUnitInfo>& getMyTemplars() { return myTemplars; }
	map <Unit, SupportUnitInfo>& getMyReavers() { return myReavers; }
	map <Unit, SupportUnitInfo>& getMyDarchons() { return myDarchons; }
};

typedef Singleton<SpecialUnitTrackerClass> SpecialUnitTracker;