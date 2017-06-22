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
	Position resourcePosition;
	TilePosition resourceTilePosition;
	UnitType unitType;
	Unit nexus, storedUnit;
	BWTA::Region* resourceRegion;
public:
	// Constructors
	ResourceInfo();
	~ResourceInfo();
	ResourceInfo(int, int, Unit, Position, TilePosition, UnitType);

	// Unit access
	Unit unit() { return storedUnit; }
	void setUnit(Unit newUnit) { storedUnit = newUnit; }

	// Accessors
	int getGathererCount() { return gathererCount; };
	int getRemainingResources() { return remainingResources; }
	Unit getClosestNexus() { return nexus; }
	Position getPosition() { return resourcePosition; }
	TilePosition getTilePosition() { return resourceTilePosition; }
	UnitType getType() { return unitType; }
	BWTA::Region* getRegion() { return resourceRegion; }

	// Mutators
	void setGathererCount(int newGathererCount) { gathererCount = newGathererCount; }
	void setRemainingResources(int newRemainingResources) { remainingResources = newRemainingResources; }
	void setClosestNexus(Unit newNexus) { nexus = newNexus; }
	void setPosition(Position newPosition) { resourcePosition = newPosition; }
	void setTilePosition(TilePosition newTilePosition) { resourceTilePosition = newTilePosition; }
	void setUnitType(UnitType newUnitType) { unitType = newUnitType; }
};