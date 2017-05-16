#include "NexusInfo.h"

// Constructors
NexusInfo::NexusInfo(int newStaticDefenseCount, TilePosition newStaticPosition)
{
	staticDefenseCount = newStaticDefenseCount;
	staticPosition = newStaticPosition;
}

// Accessors
int NexusInfo::getStaticDefenseCount() const
{
	return staticDefenseCount;
}
TilePosition NexusInfo::getStaticPosition() const
{
	return staticPosition;
}


// Mutators
void NexusInfo::setStaticDefenseCount(int newStaticDefenseCount)
{
	staticDefenseCount = newStaticDefenseCount;
}

void NexusInfo::setStaticPosition(TilePosition newPosition)
{
	staticPosition = newPosition;
}