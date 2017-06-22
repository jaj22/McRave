#include "McRave.h"

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
	trainProbes();
	updateDefenses();
}

void NexusTrackerClass::storeNexus(Unit nexus)
{
	if (myNexus.find(nexus) == myNexus.end())
	{
		myNexus[nexus].setCannonPosition(staticDefensePosition(nexus));
		myNexus[nexus].setNexusTilePosition(nexus->getTilePosition());
		myNexus[nexus].setRegion(getRegion(nexus->getTilePosition()));
		myNexus[nexus].setPosition(nexus->getPosition());
	}
}

void NexusTrackerClass::removeNexus(Unit nexus)
{
	if (myNexus.find(nexus) != myNexus.end())
	{
		myNexus.erase(nexus);
		if (Terrain().getAllyTerritory().find(getRegion(nexus->getTilePosition())) != Terrain().getAllyTerritory().end())
		{
			Terrain().getAllyTerritory().erase(getRegion(nexus->getTilePosition()));
		}
	}
}

void NexusTrackerClass::trainProbes()
{
	for (auto nexus : myNexus)
	{
		if ((!Resources().isMinSaturated() || !Resources().isGasSaturated()) && nexus.first->isIdle() && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) < 60 && (Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + Production().getReservedMineral() + Buildings().getQueuedMineral()))
		{
			nexus.first->train(UnitTypes::Protoss_Probe);
		}
	}
}

void NexusTrackerClass::updateDefenses()
{
	for (auto &nexus : myNexus)
	{
		if (!Terrain().getAnalyzed())
		{
			continue;
		}
		//if (nexus.first->getUnitsInRadius(128, Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Pylon).size() == 0 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) > 1)
		//{
		//	TilePosition here = Buildings().getBuildLocationNear(UnitTypes::Protoss_Pylon, nexus.first->getTilePosition(), false);
		//	Unit builder = Broodwar->getClosestUnit(Position(here), Filter::IsAlly && Filter::IsWorker && !Filter::IsGatheringGas && !Filter::IsCarryingGas && !Filter::IsStuck);
		//	// Create a pylon there
		//	if (here != TilePositions::None && builder)
		//	{
		//		// Queue at this building type a pair of building placement and builder
		//		Buildings().getQueuedBuildings().emplace(UnitTypes::Protoss_Pylon, make_pair(here, builder));
		//		Grids().updateReservedLocation(UnitTypes::Protoss_Pylon, here);
		//	}
		//}

		// Emplace the ally territory
		Terrain().getAllyTerritory().emplace(getRegion(nexus.first->getTilePosition()));
	}

	//	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Forge) == 0 || !nexus.first->isCompleted())
	//	{
	//		continue;
	//	}

	//	nexus.second.setCannonCount(nexus.first->getUnitsInRadius(640, Filter::GetType == UnitTypes::Protoss_Photon_Cannon).size());
	//			
	//	if (!Broodwar->hasPower(nexus.second.getCannonPosition()) && Broodwar->getUnitsInRadius(Position(nexus.second.getCannonPosition()), 256, Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Pylon).size() == 0)
	//	{
	//		Unit builder = Broodwar->getClosestUnit(Position(nexus.second.getCannonPosition()), Filter::IsAlly && Filter::IsWorker && !Filter::IsCarryingSomething && !Filter::IsGatheringGas);
	//		TilePosition here = Buildings().getBuildLocationNear(UnitTypes::Protoss_Pylon, nexus.second.getCannonPosition(), true);
	//		if (here != TilePositions::None && builder)
	//		{
	//			// Queue at this building type a pair of building placement and builder
	//			Buildings().getQueuedBuildings().emplace(UnitTypes::Protoss_Pylon, make_pair(here, builder));
	//		}
	//	}
	//	else if (nexus.second.getCannonCount() < 2 && Broodwar->hasPower(nexus.second.getCannonPosition()))
	//	{
	//		Unit builder = Broodwar->getClosestUnit(Position(nexus.second.getCannonPosition()), Filter::IsAlly && Filter::IsWorker && !Filter::IsCarryingSomething && !Filter::IsGatheringGas);
	//		TilePosition here = Buildings().getBuildLocationNear(UnitTypes::Protoss_Photon_Cannon, nexus.second.getCannonPosition(), true);
	//		if (here != TilePositions::None && builder)
	//		{
	//			// Queue at this building type a pair of building placement and builder
	//			Buildings().getQueuedBuildings().emplace(UnitTypes::Protoss_Photon_Cannon, make_pair(here, builder));
	//		}
	//	}
	//}
}