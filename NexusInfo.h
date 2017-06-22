#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class NexusInfo{
	int cannonCount;
	Unit storedUnit;
	TilePosition cannonPosition, nexusTilePosition;
	Position nexusPosition;
	BWTA::Region* region;
public:
	// Constructors
	NexusInfo();

	// Unit access
	Unit unit() { return storedUnit; }
	void setUnit(Unit newUnit) { storedUnit = newUnit; }

	// Accessors
	int getCannonCount() { return cannonCount; }
	TilePosition getCannonPosition() { return cannonPosition; }
	TilePosition getTilePosition() { return nexusTilePosition; }
	Position getPosition() { return nexusPosition; }
	BWTA::Region* getRegion() { return region; }	

	// Mutators
	void setCannonCount(int newCount) { cannonCount = newCount; }
	void setCannonPosition(TilePosition newCannonPosition) { cannonPosition = newCannonPosition; }
	void setNexusTilePosition(TilePosition newTilePosition) { nexusTilePosition = newTilePosition; }
	void setPosition(Position newPosition) { nexusPosition = newPosition; }
	void setRegion(BWTA::Region* newRegion) { region = newRegion; }
};