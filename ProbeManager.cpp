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

void assignGas(Unit probe, Unit gas)
{
	// If Probe is assigned to gas, match with Assimilator that needs the Probe
	gasProbeMap[probe] = gas;
	probe->gather(gas);
	return;
}

void assignMinerals(Unit probe, Unit mineral)
{
	// If Probe is assigned to minerals, match with mineral field that needs the Probe
	mineralProbeMap[probe].first = mineral;
	mineralProbeMap[probe].second = mineral->getPosition();
	probe->gather(mineral);
	return;
}

void assignProbe(Unit probe)
{
	int cnt = 1;	
	// See if we need gas probes
		for (auto gas : gasMap)
		{
			if (gas.second < 3)
			{
				assignGas(probe, gas.first);
				gasMap[gas.first] = gas.second + 1;
				return;
			}
		}

		/*for (auto gas : myGas)
		{
			if (gas.second.getGathererCount() < 3)
			{
				assignGas(probe, gas.first);
				gas.second.setGathererCount(gas.second.getGathererCount() + 1);
			}
		}*/
	

	// First checks if a mineral field has 0 Probes mining, if none, checks if a mineral field has 1 Probe mining. Assigns to 0 first, then 1. Spreads saturation.
	// IMPLEMENTING -- Split probes when an expansion finishes?
	gasNeeded = false;
	while (cnt <= 2)
	{
		for (auto mineral : mineralMap)
		{
			// First round on minerals
			if (mineral.second < cnt)
			{
				saturated = false;				
				assignMinerals(probe, mineral.first);
				mineralMap[mineral.first] = cnt;
				return;
			}
		}
		cnt++;	
		// If we have at least 1 Probe on every mineral, we can get another gas
		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) == Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) && Broodwar->self()->gas() < 50)
		{
			gasNeeded = true;
		}
	}
	
	// If we reached the enemy of our map and no Probes were assigned, we are saturated and don't need any more Probes
	saturated = true;

	// Any idle Probes can gather closest mineral field until they are assigned again
	if (probe->isIdle() && probe->getClosestUnit(Filter::IsMineralField))
	{
		probe->gather(probe->getClosestUnit(Filter::IsMineralField));
	}
	return;
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
ResourceInfo::ResourceInfo(int newGathererCount, int newRemainingResources, Position newPosition)
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

// Resource updating
void storeResource(Unit resource, map <Unit, ResourceInfo>& myResources)
{
	// Each resource has a probe count, can update when probe assigned/unassigned (death or otherwise)
	// Position should always be constant
	// Remaining resources can be updated every frame along with resource iterator (can monitor if expansions are needed easier)

	// If this is a new unit, initialize at 0 workers, initial resources and find position
	ResourceInfo newResource(0, resource->getInitialResources(), resource->getPosition());
}