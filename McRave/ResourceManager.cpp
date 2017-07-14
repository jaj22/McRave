#include "McRave.h"

void ResourceTrackerClass::update()
{
	Display().startClock();
	updateResources();
	Display().performanceTest(__FUNCTION__);
	return;
}

void ResourceTrackerClass::updateResources()
{
	// Assume mineral saturation, will be changed to false if any mineral field has less than 2 gatherers
	minSat = true;
	for (auto &m : myMinerals)
	{
		if (m.first->exists())
		{
			m.second.setRemainingResources(m.first->getResources());
		}
		if (minSat && m.second.getGathererCount() < 2)
		{
			minSat = false;
		}
	}

	// Assume gas saturation, will be changed to false if any gas geyser has less than 3 gatherers
	gasSat = true;
	for (auto &g : myGas)
	{
		if (g.first->exists())
		{
			g.second.setUnitType(g.first->getType());
			g.second.setRemainingResources(g.first->getResources());
		}
		if (g.second.getGathererCount() < 3 && g.second.getType() != UnitTypes::Resource_Vespene_Geyser && g.second.unit()->isCompleted())
		{
			gasNeeded = 3 - g.second.getGathererCount();
			gasSat = false;
			break;
		}
	}
	return;
}

void ResourceTrackerClass::storeMineral(Unit resource)
{	
	myMinerals[resource].setGathererCount(0);
	myMinerals[resource].setRemainingResources(resource->getResources());
	myMinerals[resource].setUnit(resource);
	myMinerals[resource].setClosestBase(resource->getClosestUnit(Filter::IsAlly && Filter::IsResourceDepot));
	myMinerals[resource].setUnitType(resource->getType());
	myMinerals[resource].setPosition(resource->getPosition());
	myMinerals[resource].setWalkPosition(Util().getWalkPosition(resource));
	myMinerals[resource].setTilePosition(resource->getTilePosition());
	return;
}

void ResourceTrackerClass::storeGas(Unit resource)
{
	myGas[resource].setGathererCount(0);
	myGas[resource].setRemainingResources(resource->getResources());
	myGas[resource].setUnit(resource);
	myGas[resource].setClosestBase(resource->getClosestUnit(Filter::IsAlly && Filter::IsResourceDepot));
	myGas[resource].setUnitType(resource->getType());
	myGas[resource].setPosition(resource->getPosition());
	myGas[resource].setWalkPosition(Util().getWalkPosition(resource));
	myGas[resource].setTilePosition(resource->getTilePosition());
	return;
}

void ResourceTrackerClass::storeBoulder(Unit resource)
{
	myBoulders[resource].setGathererCount(0);
	myBoulders[resource].setRemainingResources(resource->getResources());
	myBoulders[resource].setUnit(resource);
	myBoulders[resource].setClosestBase(resource->getClosestUnit(Filter::IsAlly && Filter::IsResourceDepot));
	myBoulders[resource].setUnitType(resource->getType());
	myBoulders[resource].setPosition(resource->getPosition());
	myBoulders[resource].setWalkPosition(Util().getWalkPosition(resource));
	myBoulders[resource].setTilePosition(resource->getTilePosition());
	return;
}

void ResourceTrackerClass::removeResource(Unit resource)
{
	// Remove dead resources
	if (myMinerals.find(resource) != myMinerals.end())
	{
		myMinerals.erase(resource);		
	}
	else if (myGas.find(resource) != myGas.end())
	{
		myGas.erase(resource);
	}
	else if (myBoulders.find(resource) != myBoulders.end())
	{
		myBoulders.erase(resource);
	}

	// Any workers that targeted that resource now have no target
	for (auto &worker : Workers().getMyWorkers())
	{
		if (worker.second.getResource() == resource)
		{
			worker.second.setResource(nullptr);
		}
	}
	return;
}
