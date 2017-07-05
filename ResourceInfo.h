#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class ResourceInfo
{
private:
	int gathererCount, remainingResources;
	Unit storedUnit, base;
	UnitType unitType;
	Position position;
	TilePosition tilePosition;
	WalkPosition walkPosition;	
public:
	ResourceInfo();
	~ResourceInfo();

	int getGathererCount() { return gathererCount; };
	int getRemainingResources() { return remainingResources; }

	Unit unit() { return storedUnit; }
	Unit getClosestBase() { return base; }
	UnitType getType() { return unitType; }

	Position getPosition() { return position; }
	WalkPosition getWalkPosition() { return walkPosition; }
	TilePosition getTilePosition() { return tilePosition; }

	void setGathererCount(int newGathererCount) { gathererCount = newGathererCount; }
	void setRemainingResources(int newRemainingResources) { remainingResources = newRemainingResources; }

	void setUnit(Unit newUnit) { storedUnit = newUnit; }
	void setClosestBase(Unit newBase) { base = newBase; }
	void setUnitType(UnitType newType) { unitType = newType; }

	void setPosition(Position newPosition) { position = newPosition; }
	void setWalkPosition(WalkPosition newPosition) { walkPosition = newPosition; }
	void setTilePosition(TilePosition newTilePosition) { tilePosition = newTilePosition; }
};