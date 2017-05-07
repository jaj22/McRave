#include "NexusManager.h"
#include "GridManager.h"
#include "ProductionManager.h"
#include "BuildingManager.h"
#include "ResourceManager.h"

TilePosition staticDefensePosition(Unit nexus)
{
	// Get average of minerals	
	int avgX = 0, avgY = 0, size = 0;
	for (auto m : Broodwar->getUnitsInRadius(nexus->getPosition(), 320, Filter::IsMineralField))
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

void NexusTrackerClass::update()
{
	storeNexus();
	trainProbes();
}

void NexusTrackerClass::storeNexus()
{
	for (auto nexus : Broodwar->self()->getUnits())
	{
		if (nexus->getType() == UnitTypes::Protoss_Nexus)
		{
			// Create new object
			NexusInfo newUnit(nexus->getUnitsInRadius(320, Filter::GetType == UnitTypes::Protoss_Photon_Cannon).size(), staticDefensePosition(nexus));
			// Store it in the map
			myNexus[nexus] = newUnit;
		}
	}
}

void NexusTrackerClass::trainProbes()
{
	for (auto nexus : myNexus)
	{
		if (!ResourceTracker::Instance().isSaturated() && nexus.first->isIdle() && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) < 60 && (Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + ProductionTracker::Instance().getReservedMineral() + BuildingTracker::Instance().getQueuedMineral()))
		{
			nexus.first->train(UnitTypes::Protoss_Probe);
		}
	}
}



