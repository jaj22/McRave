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
	// Create new object, let's say there's 5 cannons nearby initially
	NexusInfo newUnit(5);
	// Store it in the map
	myNexus[nexus].setStaticD(5);
}