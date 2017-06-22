#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class BuildingInfo
{
	int energy;
	bool idleStatus;
	Unit thisUnit;
	UnitType unitType, nextUnit;
	Position position;
	WalkPosition walkPosition;
	TilePosition tilePosition;

public:
	BuildingInfo();

	int getEnergy() { return energy; }
	bool isIdle() { return idleStatus; }
	Unit unit() { return thisUnit; }
	UnitType getUnitType() { return unitType; }
	UnitType getNextUnit() { return nextUnit; }
	Position getPosition() { return position; }
	TilePosition getTilePosition() { return tilePosition; }
	WalkPosition getWalkPosition() { return walkPosition; }
	
	void setEnergy(int newEnergy) { energy = newEnergy; }
	void setIdleStatus(bool newIdleStatus) { idleStatus = newIdleStatus; }
	void setUnit(Unit newUnit) { thisUnit = newUnit; }
	void setUnitType(UnitType newBuildingType) { unitType = newBuildingType; }
	void setNextUnit(UnitType newNextUnit) { nextUnit = newNextUnit; }
	void setPosition(Position newPosition) { position = newPosition; }
	void setWalkPosition(WalkPosition newPosition) { walkPosition = newPosition; }
	void setTilePosition(TilePosition newPosition) { tilePosition = newPosition; }
	
};