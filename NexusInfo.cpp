#include "NexusInfo.h"

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