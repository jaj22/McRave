#include "NexusInfo.h"

// Constructors
NexusInfo::NexusInfo(int newStaticDefenseCount, TilePosition newStaticPosition)
{
	staticDefenseCount = newStaticDefenseCount;
	staticPosition = newStaticPosition;
}

NexusInfo::NexusInfo()
{
	staticDefenseCount = 0;
	staticPosition = TilePositions::None;
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