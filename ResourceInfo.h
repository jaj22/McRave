#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class ResourceInfo
{
private:
	int gathererCount, remainingResources;
	Unit storedUnit, nexus;
	UnitType unitType;
	Position position;
	TilePosition tilePosition;
	WalkPosition walkPosition;	
	BWTA::Region* resourceRegion;
public:
	ResourceInfo();
	~ResourceInfo();

	int getGathererCount() { return gathererCount; };
	int getRemainingResources() { return remainingResources; }

	Unit unit() { return storedUnit; }
	Unit getClosestNexus() { return nexus; }
	UnitType getUnitType() { return unitType; }

	Position getPosition() { return position; }
	WalkPosition getWalkPosition() { return walkPosition; }
	TilePosition getTilePosition() { return tilePosition; }
	BWTA::Region* getRegion() { return resourceRegion; }

	void setGathererCount(int newGathererCount) { gathererCount = newGathererCount; }
	void setRemainingResources(int newRemainingResources) { remainingResources = newRemainingResources; }

	void setUnit(Unit newUnit) { storedUnit = newUnit; }
	void setClosestNexus(Unit newNexus) { nexus = newNexus; }
	void setUnitType(UnitType newType) { unitType = newType; }

	void setPosition(Position newPosition) { position = newPosition; }
	void setWalkPosition(WalkPosition newPosition) { walkPosition = newPosition; }
	void setTilePosition(TilePosition newTilePosition) { tilePosition = newTilePosition; }
};