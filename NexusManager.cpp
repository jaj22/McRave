#include "NexusManager.h"



// For each Nexus, count number of cannons nearby
// Further away from starting position = more cannons?

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
int NexusInfo::getStaticD() const
{
	return staticD;
}
void NexusInfo::setStaticD(int newStaticD)
{
	staticD = newStaticD;
}

void updateDefenses(Unit nexus, map <Unit, NexusInfo>& myNexus)
{
	// Create new object
	NexusInfo newUnit(nexus->getUnitsInRadius(300, Filter::GetType == UnitTypes::Protoss_Photon_Cannon).size());
	// Store it in the map
	myNexus[nexus] = newUnit;
}