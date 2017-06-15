#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class BuildingInfo
{
	bool idleStatus;
	UnitType buildingType, nextUnit;
	TilePosition buildingTilePosition;
public:
	BuildingInfo();
	~BuildingInfo();
	void setIdleStatus(bool newIdleStatus) { idleStatus = newIdleStatus; }
	void setBuildingType(UnitType newBuildingType) { buildingType = newBuildingType; }
	void setNextUnit(UnitType newNextUnit) { nextUnit = newNextUnit; }
	void setTilePosition(TilePosition newTilePosition) { buildingTilePosition = newTilePosition; }

	bool isIdle() { return idleStatus; }
	UnitType getType() { return buildingType; }
	UnitType getNextUnit() { return nextUnit; }
	TilePosition getTilePosition() { return buildingTilePosition; }
};