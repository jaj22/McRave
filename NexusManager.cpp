#include "NexusManager.h"
#include "GridManager.h"
#include "ProductionManager.h"
#include "BuildingManager.h"
#include "ResourceManager.h"
#include "TerrainManager.h"

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
	//updateDefenses();
}

void NexusTrackerClass::storeNexus()
{
	for (auto nexus : Broodwar->self()->getUnits())
	{
		if (nexus->getType() == UnitTypes::Protoss_Nexus && myNexus.find(nexus) == myNexus.end())
		{
			NexusInfo newUnit(nexus->getUnitsInRadius(320, Filter::GetType == UnitTypes::Protoss_Photon_Cannon).size(), staticDefensePosition(nexus));		
			myNexus[nexus] = newUnit;
		}
	}
}

void NexusTrackerClass::removeNexus(Unit nexus)
{
	if (myNexus.find(nexus) != myNexus.end())
	{
		myNexus.erase(nexus);
		if (TerrainTracker::Instance().getAllyTerritory().find(getRegion(nexus->getTilePosition())) != TerrainTracker::Instance().getAllyTerritory().end())
		{
			TerrainTracker::Instance().getAllyTerritory().erase(getRegion(nexus->getTilePosition()));
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

void NexusTrackerClass::updateDefenses()
{
	for (auto &nexus : myNexus)
	{
		if (!TerrainTracker::Instance().getAnalyzed())
		{
			continue;
		}

		// Emplace the ally territory
		TerrainTracker::Instance().getAllyTerritory().emplace(getRegion(nexus.first->getTilePosition()));


		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Forge) == 0 || !nexus.first->isCompleted())
		{
			continue;
		}

		nexus.second.setCannonCount(nexus.first->getUnitsInRadius(640, Filter::GetType == UnitTypes::Protoss_Photon_Cannon).size());
				
		if (!Broodwar->hasPower(nexus.second.getCannonPosition()) && Broodwar->getUnitsInRadius(Position(nexus.second.getCannonPosition()), 256, Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Pylon).size() == 0)
		{
			Unit builder = Broodwar->getClosestUnit(Position(nexus.second.getCannonPosition()), Filter::IsAlly && Filter::IsWorker && !Filter::IsCarryingSomething && !Filter::IsGatheringGas);
			TilePosition here = BuildingTracker::Instance().getBuildLocationNear(UnitTypes::Protoss_Pylon, nexus.second.getCannonPosition(), true);
			if (here != TilePositions::None && builder)
			{
				// Queue at this building type a pair of building placement and builder
				BuildingTracker::Instance().getQueuedBuildings().emplace(UnitTypes::Protoss_Pylon, make_pair(here, builder));
			}
		}
		else if (nexus.second.getCannonCount() < 2 && Broodwar->hasPower(nexus.second.getCannonPosition()))
		{
			Unit builder = Broodwar->getClosestUnit(Position(nexus.second.getCannonPosition()), Filter::IsAlly && Filter::IsWorker && !Filter::IsCarryingSomething && !Filter::IsGatheringGas);
			TilePosition here = BuildingTracker::Instance().getBuildLocationNear(UnitTypes::Protoss_Photon_Cannon, nexus.second.getCannonPosition(), true);
			if (here != TilePositions::None && builder)
			{
				// Queue at this building type a pair of building placement and builder
				BuildingTracker::Instance().getQueuedBuildings().emplace(UnitTypes::Protoss_Photon_Cannon, make_pair(here, builder));
			}
		}
	}
}


