#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class NexusInfo{
	int cannonCount;
	TilePosition cannonPosition, nexusTilePosition;
	BWTA::Region* region;
public:
	// Constructors
	NexusInfo();
	NexusInfo(int, TilePosition, TilePosition);

	// Accessors
	int getCannonCount();
	TilePosition getCannonPosition();
	BWTA::Region* getRegion();
	TilePosition getTilePosition() { return nexusTilePosition; }

	// Mutators
	void setCannonCount(int newCount);
	void setCannonPosition(TilePosition newTile);
	void setRegion(BWTA::Region* newRegion);
	void setNexusTilePosition(TilePosition newTilePosition) { nexusTilePosition = newTilePosition; }
};