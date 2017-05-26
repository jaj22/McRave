#include "NexusInfo.h"

// Constructors
NexusInfo::NexusInfo(int newCannonCount, TilePosition newStaticPosition)
{
	cannonCount = newCannonCount;
	cannonPosition = newStaticPosition;
}

NexusInfo::NexusInfo()
{
	cannonCount = 0;
	cannonPosition = TilePositions::None;
}

// Accessors
int NexusInfo::getCannonCount()
{
	return cannonCount;
}
TilePosition NexusInfo::getCannonPosition()
{
	return cannonPosition;
}


// Mutators
void NexusInfo::setCannonCount(int newCannonCount)
{
	cannonCount = newCannonCount;
}

void NexusInfo::setCannonPosition(TilePosition newPosition)
{
	cannonPosition = newPosition;
}