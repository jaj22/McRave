#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace std;

class BaseInfo{
	int defenseCount;
	Unit thisUnit;
	UnitType unitType;
	Position position;
	WalkPosition walkPosition;
	TilePosition resourcesPosition, tilePosition;
public:
	BaseInfo();	

	int getDefenseCount() { return defenseCount; }
	Unit unit() { return thisUnit; }
	UnitType getType() { return unitType; }
	Position getPosition() { return position; }
	WalkPosition getWalkPosition() { return walkPosition; }
	TilePosition getTilePosition() { return tilePosition; }
	TilePosition getResourcesPosition() { return resourcesPosition; }	

	void setDefenseCount(int newCount) { defenseCount = newCount; }
	void setUnitType(UnitType newType) { unitType = newType; }
	void setUnit(Unit newUnit) { thisUnit = newUnit; }
	void setPosition(Position newPosition) { position = newPosition; }
	void setWalkPosition(WalkPosition newPosition) { walkPosition = newPosition; }
	void setTilePosition(TilePosition newPosition) { tilePosition = newPosition; }
	void setResourcesPosition(TilePosition newPosition) { resourcesPosition = newPosition; }	
};