#include "ProbeManager.h"

using namespace BWAPI;
using namespace std;

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

// Probe updating

void storeProbe(Unit probe, map <Unit, ProbeInfo>& myProbes)
{
	ProbeInfo newProbe;
	myProbes[probe] = newProbe;
	return;
}