#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class NexusInfo{
	int cannonCount;
	TilePosition cannonPosition, pylonPosition;
	BWTA::Region* region;
public:
	// Constructors
	NexusInfo();
	NexusInfo(int, TilePosition);

	// Accessors
	int getCannonCount();
	TilePosition getCannonPosition();
	BWTA::Region* getRegion();


	// Mutators
	void setCannonCount(int newCount);
	void setCannonPosition(TilePosition newTile);
	void setRegion(BWTA::Region* newRegion);
};