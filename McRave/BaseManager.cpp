#include "McRave.h"

void BaseTrackerClass::update()
{
	Display().startClock();
	updateAlliedBases();
	Display().performanceTest(__FUNCTION__);
	return;
}

void BaseTrackerClass::updateAlliedBases()
{
	for (auto &base : myBases)
	{
		if ((base.first && !base.first->exists()) || !base.first)
		{
			myBases.erase(base.first);
			break;
		}
		if (base.second.unit() && base.second.unit()->exists())
		{
			trainWorkers(base.second);
			updateDefenses(base.second);
		}
	}
	return;
}

void BaseTrackerClass::storeBase(Unit base)
{
	myBases[base].setUnit(base);
	myBases[base].setUnitType(base->getType());
	myBases[base].setResourcesPosition(centerOfResources(base));
	myBases[base].setPosition(base->getPosition());
	myBases[base].setWalkPosition(Util().getWalkPosition(base));
	myBases[base].setTilePosition(base->getTilePosition());
	myBases[base].setPosition(base->getPosition());
	myOrderedBases[base->getPosition().getDistance(Terrain().getPlayerStartingPosition())] = base->getTilePosition();
	return;
}

void BaseTrackerClass::removeBase(Unit base)
{
	myOrderedBases.erase(base->getPosition().getDistance(Terrain().getPlayerStartingPosition()));
	myBases.erase(base);
	return;
}

void BaseTrackerClass::trainWorkers(BaseInfo& base)
{
	if (base.unit() && (!Resources().isMinSaturated() || !Resources().isGasSaturated()) && base.unit()->isIdle())
	{
		for (auto &worker : base.getType().buildsWhat())
		{			
			if (Broodwar->self()->completedUnitCount(worker) < 60 && (Broodwar->self()->minerals() >= worker.mineralPrice() + Production().getReservedMineral() + Buildings().getQueuedMineral()))
			{
				base.unit()->train(worker);
			}
		}		
	}
	return;
}

void BaseTrackerClass::updateDefenses(BaseInfo& base)
{
	// Update defenses got gutted, this can be merged somewhere else
	Terrain().getAllyTerritory().emplace(theMap.GetArea(base.getTilePosition())->Id());
	return;
}

TilePosition BaseTrackerClass::centerOfResources(Unit base)
{
	// Get average of minerals	
	int avgX = 0, avgY = 0, size = 0;
	for (auto &m : Broodwar->getUnitsInRadius(base->getPosition(), 320, Filter::IsMineralField))
	{
		avgX = avgX + m->getTilePosition().x;
		avgY = avgY + m->getTilePosition().y;
		size++;
	}
	if (size == 0)
	{
		return TilePositions::None;
	}

	avgX = avgX / size;
	avgY = avgY / size;

	return TilePosition(avgX, avgY);
}