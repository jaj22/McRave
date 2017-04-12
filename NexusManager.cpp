#include "NexusManager.h"

// Constructors
NexusInfo::NexusInfo()
{
	staticD = 0;
}
NexusInfo::~NexusInfo()
{

}
NexusInfo::NexusInfo(int newStaticD)
{
	staticD = newStaticD;
}

// Accessors
int NexusInfo::getStaticD() const
{
	return staticD;
}

// Mutators
void NexusInfo::setStaticD(int newStaticD)
{
	staticD = newStaticD;
}

// Defense updating
void updateDefenses(Unit nexus, map <Unit, NexusInfo>& myNexus)
{
	// Create new object
	NexusInfo newUnit(nexus->getUnitsInRadius(300, Filter::GetType == UnitTypes::Protoss_Photon_Cannon).size());
	// Store it in the map
	myNexus[nexus] = newUnit;
}