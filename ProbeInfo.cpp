#include "ProbeInfo.h"


void storeProbe(Unit probe, map <Unit, ProbeInfo>& myProbes)
{
	ProbeInfo newProbe;
	myProbes[probe] = newProbe;
	return;
}

// Constructors
ProbeInfo::ProbeInfo()
{
	target = nullptr;
}
ProbeInfo::~ProbeInfo()
{

}
ProbeInfo::ProbeInfo(Unit newTarget)
{
	target = newTarget;
}

// Accessors
Unit ProbeInfo::getTarget() const
{
	return target;
}

// Mutators
void ProbeInfo::setTarget(Unit newTarget)
{
	target = newTarget;
}