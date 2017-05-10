#include "NexusInfo.h"

// Constructors
NexusInfo::NexusInfo()
{
	staticDefenseCount = 0;
	staticPosition = TilePositions::None;
	pylon = nullptr;
}
NexusInfo::~NexusInfo()
{

}
NexusInfo::NexusInfo(int newStaticDefenseCount, TilePosition newStaticPosition, Unit newPylon)
{
	staticDefenseCount = newStaticDefenseCount;
	staticPosition = newStaticPosition;
	pylon = newPylon;
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
Unit NexusInfo::getPylon() const
{
	return pylon;
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
void NexusInfo::setPylon(Unit newPylon)
{
	pylon = newPylon;
}