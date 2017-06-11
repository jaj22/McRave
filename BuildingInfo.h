#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;
using namespace UnitTypes;

class BuildingInfoClass
{
	bool idleStatus;
	UnitType buildingType, nextUnit;
	TilePosition buildingTilePosition;
public:
	BuildingInfoClass();
	~BuildingInfoClass();
	void setIdleStatus(bool newIdleStatus) { idleStatus = newIdleStatus; }
	void setBuildingType(UnitType newBuildingType) { buildingType = newBuildingType; }
	void setNextUnit(UnitType newNextUnit) { nextUnit = newNextUnit; }
	void setTilePosition(TilePosition newTilePosition) { buildingTilePosition = newTilePosition; }

	bool isIdle() { return idleStatus; }
	UnitType getType() { return buildingType; }
	UnitType getNextUnit() { return nextUnit; }
	TilePosition getTilePosition() { return buildingTilePosition; }
};