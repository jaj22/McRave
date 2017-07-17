#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class ResourceInfo
{
private:
	int gathererCount, remainingResources;
	Unit storedUnit;
	UnitType unitType;
	Position position, resourceClusterPosition, baseCenter;
	TilePosition tilePosition;
	WalkPosition walkPosition;

public:
	ResourceInfo();
	~ResourceInfo();

	int getGathererCount() { return gathererCount; };
	int getRemainingResources() { return remainingResources; }

	Unit unit() { return storedUnit; }
	UnitType getType() { return unitType; }

	Position getPosition() { return position; }
	Position getResourceClusterPosition() { return resourceClusterPosition; }
	Position getClosestBasePosition() { return baseCenter; }
	WalkPosition getWalkPosition() { return walkPosition; }	
	TilePosition getTilePosition() { return tilePosition; }

	void setGathererCount(int newGathererCount) { gathererCount = newGathererCount; }
	void setRemainingResources(int newRemainingResources) { remainingResources = newRemainingResources; }

	void setUnit(Unit newUnit) { storedUnit = newUnit; }
	void setUnitType(UnitType newType) { unitType = newType; }

	void setPosition(Position newPosition) { position = newPosition; }
	void setResourceClusterPosition(Position newPosition) { resourceClusterPosition = newPosition; }
	void setClosestBasePosition(Position newPosition) { baseCenter = newPosition; }
	void setWalkPosition(WalkPosition newPosition) { walkPosition = newPosition; }	
	void setTilePosition(TilePosition newTilePosition) { tilePosition = newTilePosition; }
};