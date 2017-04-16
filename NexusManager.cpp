#include "NexusManager.h"

// Constructors
NexusInfo::NexusInfo()
{
	staticD = 0;
}
NexusInfo::~NexusInfo()
{

}
NexusInfo::NexusInfo(int newStaticD, TilePosition newStaticP)
{
	staticD = newStaticD;
	staticP = newStaticP;
}

// Accessors
int NexusInfo::getStaticD() const
{
	return staticD;
}
TilePosition NexusInfo::getStaticP() const
{
	return staticP;
}

// Mutators
void NexusInfo::setStaticD(int newStaticD)
{
	staticD = newStaticD;
}

TilePosition staticDefensePosition(Unit nexus)
{
	// Get average of minerals	
	int avgX = 0, avgY = 0, size = 0;
	for (auto m : Broodwar->getUnitsInRadius(nexus->getPosition(), 320, Filter::IsMineralField))
	{
		avgX = avgX + m->getTilePosition().x;
		avgY = avgY + m->getTilePosition().y;
		size++;
	}
	if (size == 0)
	{
		return TilePositions::None;
	}

	avgX = avgX / size;
	avgY = avgY / size;

	return TilePosition(avgX, avgY);
}

// Defense updating
void updateDefenses(Unit nexus, map <Unit, NexusInfo>& myNexus)
{
	// Create new object
	NexusInfo newUnit(nexus->getUnitsInRadius(320, Filter::GetType == UnitTypes::Protoss_Photon_Cannon).size(), staticDefensePosition(nexus));
	// Store it in the map
	myNexus[nexus] = newUnit;
}