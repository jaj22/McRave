#include "ResourceManager.h"
#include "TerrainManager.h"
#include "GridManager.h"
#include "ProbeManager.h"

void ResourceTrackerClass::update()
{
	for (auto &r : Broodwar->neutral()->getUnits())
	{
		if (r && r->exists())
		{
			// Need better solution than GUIR
			if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) > 0 && r->getUnitsInRadius(320, Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Nexus).size() > 0)
			{
				if (r->getType().isMineralField() && r->getInitialResources() > 0 && myMinerals.find(r) == myMinerals.end())
				{
					storeMineral(r);
				}

				if (myGas.find(r) == myGas.end() && r->getType() == UnitTypes::Resource_Vespene_Geyser)
				{
					storeGas(r);
				}				
			}
			if (r->getInitialResources() == 0 && r->getDistance(TerrainTracker::Instance().getPlayerStartingPosition()) < 2560)
			{
				storeBoulder(r);
			}
		}
	}

	// Assume saturated so check happens
	saturated = true;
	for (auto &m : myMinerals)
	{
		if (m.first->exists())
		{
			m.second.setRemainingResources(m.first->getResources());				
		}
		if (saturated && m.second.getGathererCount() < 2)
		{
			saturated = false;
		}
	}

	for (auto &g : myGas)
	{
		if (g.first->exists())
		{
			g.second.setUnitType(g.first->getType());
			g.second.setRemainingResources(g.first->getResources());			
		}
		if (g.second.getGathererCount() < 3)
		{
			saturated = false;
			break;
		}
	}
}

void ResourceTrackerClass::storeMineral(Unit resource)
{
	// If this is a new unit, initialize at 0 workers, initial resources and find position
	ResourceInfo newResource(0, resource->getInitialResources(), resource->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Nexus), resource->getPosition(), resource->getTilePosition(), resource->getType());
	myMinerals[resource] = newResource;
	return;
}

void ResourceTrackerClass::storeGas(Unit resource)
{
	// If this is a new unit, initialize at 0 workers, initial resources and find position
	ResourceInfo newResource(0, resource->getInitialResources(), resource->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Nexus), resource->getPosition(), resource->getTilePosition(), resource->getType());
	myGas[resource] = newResource;
	return;
}

void ResourceTrackerClass::storeBoulder(Unit resource)
{
	ResourceInfo newResource(0, 0, resource->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Nexus), resource->getPosition(), resource->getTilePosition(), resource->getType());
	myBoulders[resource] = newResource;
	return;
}

void ResourceTrackerClass::removeResource(Unit resource)
{
	if (myMinerals.find(resource) != myMinerals.end())
	{
		myMinerals.erase(resource);		
	}
	if (myGas.find(resource) != myGas.end())
	{
		myGas.erase(resource);
	}
	if (myBoulders.find(resource) != myBoulders.end())
	{
		myBoulders.erase(resource);
	}

	for (auto probe : ProbeTracker::Instance().getMyProbes())
	{
		if (probe.second.getTarget() == resource)
		{
			probe.second.setTarget(nullptr);
		}
	}
}
