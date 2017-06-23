#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class BaseInfo{
	int defenseCount;
	Unit thisUnit;
	UnitType unitType;
	Position position;
	TilePosition resourcesPosition, tilePosition;
	BWTA::Region* region;
public:
	BaseInfo();	

	int getDefenseCount() { return defenseCount; }
	Unit unit() { return thisUnit; }
	UnitType getUnitType() { return unitType; }
	Position getPosition() { return position; }
	TilePosition getTilePosition() { return tilePosition; }
	TilePosition getResourcesPosition() { return resourcesPosition; }
	BWTA::Region* getRegion() { return region; }

	void setDefenseCount(int newCount) { defenseCount = newCount; }
	void setUnitType(UnitType newType) { unitType = newType; }
	void setUnit(Unit newUnit) { thisUnit = newUnit; }
	void setPosition(Position newPosition) { position = newPosition; }
	void setTilePosition(TilePosition newPosition) { tilePosition = newPosition; }
	void setResourcesPosition(TilePosition newPosition) { resourcesPosition = newPosition; }	
	void setRegion(BWTA::Region* newRegion) { region = newRegion; }
};