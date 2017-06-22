#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class BuildingInfo
{
	bool idleStatus;
	UnitType buildingType, nextUnit;
	Unit storedUnit;
	TilePosition buildingTilePosition;
	int energy;
public:
	BuildingInfo();
	~BuildingInfo();

	// Unit access
	Unit unit() { return storedUnit; }
	void setUnit(Unit newUnit) { storedUnit = newUnit; }

	void setIdleStatus(bool newIdleStatus) { idleStatus = newIdleStatus; }
	void setBuildingType(UnitType newBuildingType) { buildingType = newBuildingType; }
	void setNextUnit(UnitType newNextUnit) { nextUnit = newNextUnit; }
	void setTilePosition(TilePosition newTilePosition) { buildingTilePosition = newTilePosition; }
	void setEnergy(int newEnergy) { energy = newEnergy; }

	bool isIdle() { return idleStatus; }
	UnitType getType() { return buildingType; }
	UnitType getNextUnit() { return nextUnit; }
	TilePosition getTilePosition() { return buildingTilePosition; }
	int getEnergy() { return energy; }
};