#include "ProbeManager.h"

using namespace BWAPI;
using namespace std;

void assignCombat(Unit probe)
{
	// If Probe is assigned to combat and not found, push into vector
	if (find(combatProbe.begin(), combatProbe.end(), probe) == combatProbe.end())
	{
		combatProbe.push_back(probe);
	}
	return;

}

void unAssignCombat(Unit probe)
{
	// If Probe is un assigned from combat and found, erase from vector
	combatProbe.erase(find(combatProbe.begin(), combatProbe.end(), probe));
	return;
}

Unit assignGas(Unit probe, Unit gas)
{
	// If Probe is assigned to gas, match with Assimilator that needs the Probe	
	return gas;
}

Unit assignMinerals(Unit probe, Unit mineral)
{
	// If Probe is assigned to minerals, match with mineral field that needs the Probe	
	return mineral;
}

Unit assignProbe(Unit probe)
{
	int cnt = 1;
	for (auto &gas : myGas)
	{
		if (gas.second.getUnitType() == UnitTypes::Protoss_Assimilator && gas.first->isCompleted() && gas.second.getGathererCount() < 3)
		{
			gas.second.setGathererCount(gas.second.getGathererCount() + 1);
			return assignGas(probe, gas.first);
		}
	}


	// First checks if a mineral field has 0 Probes mining, if none, checks if a mineral field has 1 Probe mining. Assigns to 0 first, then 1. Spreads saturation.
	// IMPLEMENTING -- Split probes when an expansion finishes?	
	while (cnt <= 2)
	{
		for (auto &mineral : myMinerals)
		{
			if (mineral.second.getGathererCount() < cnt)
			{
				mineral.second.setGathererCount(mineral.second.getGathererCount() + 1);
				return assignMinerals(probe, mineral.first);
			}
		}
		cnt++;
	}

	// Any idle Probes can gather closest mineral field until they are assigned again
	if (probe->isIdle() && probe->getClosestUnit(Filter::IsMineralField))
	{
		probe->gather(probe->getClosestUnit(Filter::IsMineralField));
	}
	return nullptr;
}

// Constructors
ResourceInfo::ResourceInfo()
{
	gathererCount = 0;
	remainingResources = 0;
	resourcePosition = Positions::None;
}
ResourceInfo::~ResourceInfo()
{

}
ResourceInfo::ResourceInfo(int newGathererCount, int newRemainingResources, Position newPosition, UnitType newUnitType)
{
	gathererCount = newGathererCount;
	remainingResources = newRemainingResources;
	resourcePosition = newPosition;
}

// Accessors
int ResourceInfo::getGathererCount() const
{
	return gathererCount;
}
int ResourceInfo::getRemainingResources() const
{
	return remainingResources;
}
Position ResourceInfo::getPosition() const
{
	return resourcePosition;
}
UnitType ResourceInfo::getUnitType() const
{
	return unitType;
}

// Mutators
void ResourceInfo::setGathererCount(int newGathererCount)
{
	gathererCount = newGathererCount;
}
void ResourceInfo::setRemainingResources(int newRemainingResources)
{
	remainingResources = newRemainingResources;
}
void ResourceInfo::setPosition(Position newResourcePosition)
{
	resourcePosition = newResourcePosition;
}
void ResourceInfo::setUnitType(UnitType newUnitType)
{
	unitType = newUnitType;
}

// Resource updating
void storeMineral(Unit resource, map <Unit, ResourceInfo>& myMinerals)
{
	// Each resource has a probe count, can update when probe assigned/unassigned (death or otherwise)
	// Position should always be constant
	// Remaining resources can be updated every frame along with resource iterator (can monitor if expansions are needed easier)

	// If this is a new unit, initialize at 0 workers, initial resources and find position
	ResourceInfo newResource(0, resource->getInitialResources(), resource->getPosition(), resource->getType());
	myMinerals[resource] = newResource;
	return;
}

void storeGas(Unit resource, map <Unit, ResourceInfo>& myGas)
{
	// Each resource has a probe count, can update when probe assigned/unassigned (death or otherwise)
	// Position should always be constant
	// Remaining resources can be updated every frame along with resource iterator (can monitor if expansions are needed easier)

	// If this is a new unit, initialize at 0 workers, initial resources and find position
	ResourceInfo newResource(0, resource->getInitialResources(), resource->getPosition(), resource->getType());
	myGas[resource] = newResource;
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

// Probe updating

void storeProbe(Unit probe, map <Unit, ProbeInfo>& myProbes)
{
	ProbeInfo newProbe(assignProbe(probe));
	myProbes[probe] = newProbe;
	return;
}