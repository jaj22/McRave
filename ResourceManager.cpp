#include "ResourceManager.h"

void ResourceTrackerClass::update()
{
	for (auto &r : Broodwar->neutral()->getUnits())
	{
		if (r && r->exists())
		{
			if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) > 0 && /*(allyTerritory.find(getRegion(r->getTilePosition())) != allyTerritory.end() ||*/ (Broodwar->getFrameCount() > 5 && Broodwar->getFrameCount() < 50))
			{
				if (r->getType().isMineralField() && myMinerals.find(r) == myMinerals.end() && r->getInitialResources() > 0)
				{
					storeMineral(r);
				}

				if (myGas.find(r) == myGas.end() && r->getType() == UnitTypes::Resource_Vespene_Geyser)
				{
					storeGas(r);
				}
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
			// Update resource grid
			for (int x = m.second.getTilePosition().x - 2; x <= m.second.getTilePosition().x + m.second.getUnitType().tileWidth() + 2; x++)
			{
				for (int y = m.second.getTilePosition().y - 2; y <= m.second.getTilePosition().y + m.second.getUnitType().tileHeight() + 2; y++)
				{
					if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && m.second.getPosition().getDistance(m.second.getClosestNexus()->getPosition()) > Position(x * 32, y * 32).getDistance(m.second.getClosestNexus()->getPosition()))
					{
						//resourceGrid[x][y] = 1;
					}
				}
			}
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

			// Update resource grid
			for (int x = g.second.getTilePosition().x - 1; x <= g.second.getTilePosition().x + g.second.getUnitType().tileWidth() + 1; x++)
			{
				for (int y = g.second.getTilePosition().y - 1; y <= g.second.getTilePosition().y + g.second.getUnitType().tileHeight() + 1; y++)
				{
					if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && g.second.getPosition().getDistance(g.second.getClosestNexus()->getPosition()) > Position(x * 32, y * 32).getDistance(g.second.getClosestNexus()->getPosition()))
					{
						//resourceGrid[x][y] = 1;
					}
				}
			}
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

void ResourceTrackerClass::removeMineral(Unit resource)
{
	myMinerals.erase(resource);
}

void ResourceTrackerClass::removeGas(Unit resource)
{
	
	myGas.erase(resource);
}