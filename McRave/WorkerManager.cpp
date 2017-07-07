#include "McRave.h"

void WorkerTrackerClass::update()
{
	updateScout();
	updateWorkers();
	Display().performanceTest(__func__);
	return;
}

void WorkerTrackerClass::updateWorkers()
{
	for (auto &worker : myWorkers)
	{
		updateSituational(worker.second);
		updateGathering(worker.second);
	}
}

void WorkerTrackerClass::updateScout()
{
	// Update scout probes decision if we are above 9 supply
	if (Units().getSupply() >= 18 && (!scout || (scout && !scout->exists())))
	{
		scout = getClosestWorker(Position(Terrain().getSecondChoke()));
	}	
	return;
}

void WorkerTrackerClass::exploreArea(Unit worker)
{
	WalkPosition start = myWorkers[worker].getWalkPosition();
	Position bestPosition = Terrain().getEnemyStartingPosition();
	double closestD = 1000;
	recentExplorations[start] = Broodwar->getFrameCount();

	for (int x = start.x - 10; x < start.x + 10 + worker->getType().tileWidth(); x++)
	{
		for (int y = start.y - 10; y < start.y + 10 + worker->getType().tileHeight(); y++)
		{
			if (WalkPosition(x, y).isValid() && Grids().getAntiMobilityGrid(x, y) == 0 && Grids().getMobilityGrid(x, y) > 0 && Grids().getEGroundDistanceGrid(x, y) == 0.0 && Broodwar->getFrameCount() - recentExplorations[WalkPosition(x, y)] > 1500)
			{
				if (Position(WalkPosition(x, y)).getDistance(Terrain().getEnemyStartingPosition()) < closestD)
				{
					bestPosition = Position(WalkPosition(x, y));
					closestD = Position(WalkPosition(x, y)).getDistance(Terrain().getEnemyStartingPosition());
				}
			}
		}
	}
	if (bestPosition.isValid())
	{
		worker->move(bestPosition);
	}
	return;
}

void WorkerTrackerClass::updateSituational(WorkerInfo& worker)
{
	// Scout logic
	if (scout && worker.unit() == scout && !worker.unit()->isCarryingMinerals() && worker.getBuildingType() == UnitTypes::None)
	{
		if (Terrain().getEnemyBasePositions().size() == 0 && Units().getSupply() >= 18 && scouting)
		{
			for (auto &start : theMap.StartingLocations())
			{
				if (Broodwar->isExplored(start) == false)
				{
					if (worker.unit()->getOrderTargetPosition() != Position(start))
					{
						worker.unit()->move(Position(start));
					}
					break;
				}
			}
		}
		if (Terrain().getEnemyBasePositions().size() > 0)
		{
			exploreArea(worker.unit());
		}
	}

	// Reassignment logic
	if (Resources().getGasNeeded() > 0 && (!Strategy().isRush() || Broodwar->self()->getRace() == Races::Terran))
	{
		reAssignWorker(worker);
		Resources().setGasNeeded(Resources().getGasNeeded() - 1);
	}

	// Boulder removal logic
	if (Resources().getMyBoulders().size() > 0 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) >= 2)
	{
		for (auto &b : Resources().getMyBoulders())
		{
			if (b.first && b.first->exists() && !worker.unit()->isCarryingMinerals() && worker.unit()->getDistance(b.second.getPosition()) < 320)
			{
				if (worker.unit()->getOrderTargetPosition() != b.second.getPosition() && !worker.unit()->isGatheringMinerals())
				{
					worker.unit()->gather(b.first);
				}
				return;
			}
		}
	}

	// Building logic
	if (worker.getBuildingType().isValid() && worker.getBuildPosition().isValid())
	{
		if (!Buildings().canBuildHere(worker.getBuildingType(), worker.getBuildPosition()))
		{
			worker.setBuildingType(UnitTypes::None);
			worker.setBuildPosition(TilePositions::None);
			if (!worker.unit()->isConstructing())
			{
				worker.unit()->stop();
			}
			return;
		}
		else
		{
			Grids().updateReservedLocation(worker.getBuildingType(), worker.getBuildPosition());

			if (!Broodwar->isVisible(worker.getBuildPosition()) || Broodwar->self()->minerals() >= worker.getBuildingType().mineralPrice() / worker.getPosition().getDistance(Position(worker.getBuildPosition())) && Broodwar->self()->minerals() <= worker.getBuildingType().mineralPrice() && Broodwar->self()->gas() >= worker.getBuildingType().gasPrice() / worker.getPosition().getDistance(Position(worker.getBuildPosition())) && Broodwar->self()->gas() <= worker.getBuildingType().gasPrice())
			{
				if (worker.unit()->getOrderTargetPosition() != Position(worker.getBuildPosition()) || worker.unit()->isStuck())
				{
					worker.unit()->move(Position(worker.getBuildPosition()));
				}
				return;
			}
			else if (Broodwar->self()->minerals() >= worker.getBuildingType().mineralPrice() && Broodwar->self()->gas() >= worker.getBuildingType().gasPrice())
			{
				if (worker.unit()->getOrderTargetPosition() != Position(worker.getBuildPosition()) || worker.unit()->isStuck())
				{
					worker.unit()->build(worker.getBuildingType(), worker.getBuildPosition());
				}
				return;
			}
		}
	}

	// Shield Battery logic
	if (worker.unit()->getShields() <= 5 && Grids().getBatteryGrid(worker.unit()->getTilePosition()) > 0)
	{
		if (worker.unit()->getLastCommand().getType() != UnitCommandTypes::Right_Click_Unit && worker.unit()->getClosestUnit(Filter::IsAlly && Filter::IsCompleted  && Filter::GetType == UnitTypes::Protoss_Shield_Battery && Filter::Energy > 10))
		{
			worker.unit()->rightClick(worker.unit()->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Shield_Battery));
			return;
		}
	}

	// Bunker logic
	if (Grids().getBunkerGrid(worker.getTilePosition()) > 0)
	{
		Unit bunker = worker.unit()->getClosestUnit(Filter::GetType == UnitTypes::Terran_Bunker && Filter::HP_Percent < 100);
		if (bunker)
		{
			worker.unit()->repair(bunker);
			return;
		}
	}

	// If we are fast expanding and enemy is rushing, we need to defend with workers
	if (Strategy().isFastExpand() && (Strategy().globalAlly() + Strategy().getAllyDefense()) < Strategy().globalEnemy())
	{
		Unit cannon = worker.unit()->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Photon_Cannon);
		if (cannon && cannon->exists())
		{
			Strategy().increaseGlobalAlly(2);
			Unit target = cannon->getClosestUnit(Filter::IsEnemy && !Filter::IsFlyer);
			if (target && target->exists())
			{
				worker.unit()->attack(target);
			}
			else
			{
				worker.unit()->attack(cannon->getPosition());
			}
			return;
		}
	}

	// Defending logic
	if (Broodwar->getFrameCount() - worker.getLastGatherFrame() <= 25 && Grids().getEGroundDistanceGrid(worker.getWalkPosition()) > 0)
	{
		if (!worker.getTarget() || (worker.getTarget() && !worker.getTarget()->exists()))
		{
			worker.setTarget(worker.unit()->getClosestUnit(Filter::IsEnemy && !Filter::IsFlying, 320));
		}
		else if (worker.getTarget()->exists() && (Grids().getResourceGrid(worker.getTarget()->getTilePosition()) > 0))
		{
			if ((worker.unit()->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && worker.unit()->getLastCommand().getTarget() && !worker.unit()->getLastCommand().getTarget()->exists()) || (worker.unit()->getLastCommand().getType() != UnitCommandTypes::Attack_Unit))
			{
				worker.unit()->attack(worker.getTarget());
			}
			return;
		}
	}
}

void WorkerTrackerClass::updateGathering(WorkerInfo& worker)
{
	// If we gave the worker a command already from situational behaviors, return
	if (worker.unit()->getLastCommandFrame() >= Broodwar->getFrameCount())
	{
		return;
	}

	// If worker doesn't have an assigned resource, assign one
	if (!worker.getResource())
	{
		assignWorker(worker);
		// Any idle workers can gather closest mineral field until they are assigned again
		if (worker.unit()->isIdle() && worker.unit()->getClosestUnit(Filter::IsMineralField))
		{
			worker.unit()->gather(worker.unit()->getClosestUnit(Filter::IsMineralField));
			worker.setTarget(nullptr);
			return;
		}
	}
	
	// If idle and carrying gas or minerals, return cargo			
	if (worker.unit()->isCarryingGas() || worker.unit()->isCarryingMinerals())
	{
		if (worker.unit()->getLastCommand().getType() != UnitCommandTypes::Return_Cargo)
		{
			worker.unit()->returnCargo();
		}
		return;
	}
	
	// If not targeting the mineral field the worker is mapped to
	if (!worker.unit()->isCarryingGas() && !worker.unit()->isCarryingMinerals())
	{
		if ((worker.unit()->isGatheringMinerals() || worker.unit()->isGatheringGas()) && worker.unit()->getTarget() == worker.getResource())
		{
			worker.setLastGatherFrame(Broodwar->getFrameCount());
			return;
		}
		// If the workers current target has a resource count of 0 (mineral blocking a ramp), let worker continue mining it
		if (worker.unit()->getTarget() && worker.unit()->getTarget()->getType().isMineralField() && worker.unit()->getTarget()->getResources() == 0)
		{
			return;
		}
		// If the mineral field is in vision and no target, force to gather from the assigned mineral field
		if (worker.getResource() && Grids().getBaseGrid(TilePosition(worker.getResourcePosition())) == 2)
		{
			if (worker.getResource()->exists())
			{
				worker.unit()->gather(worker.getResource());
				worker.setLastGatherFrame(Broodwar->getFrameCount());
				return;
			}
			else
			{
				worker.unit()->move(worker.getResourcePosition());
				return;
			}
		}
	}
}

Unit WorkerTrackerClass::getClosestWorker(Position here)
{
	// Currently gets the closest worker that doesn't mine gas
	Unit closestWorker = nullptr;
	double closestD = 0.0;
	for (auto &worker : myWorkers)
	{
		if (worker.first == scout)
		{
			continue;
		}
		if (worker.second.getResource() && worker.second.getResource()->exists() && !worker.second.getResource()->getType().isMineralField())
		{
			continue;
		}
		if (worker.second.getBuildingType() != UnitTypes::None)
		{
			continue;
		}
		if (worker.first->getLastCommand().getType() == UnitCommandTypes::Gather && worker.first->getLastCommand().getTarget()->exists() && worker.first->getLastCommand().getTarget()->getInitialResources() == 0)
		{
			continue;
		}
		if (worker.first->getDistance(here) < closestD || closestD == 0.0)
		{
			closestWorker = worker.first;
			closestD = worker.first->getDistance(here);
		}
	}
	return closestWorker;
}

void WorkerTrackerClass::storeWorker(Unit unit)
{
	if (unit->exists() && unit->isCompleted())
	{
		myWorkers[unit].setUnit(unit);
		myWorkers[unit].setPosition(unit->getPosition());
		myWorkers[unit].setWalkPosition(Util().getWalkPosition(unit));
		myWorkers[unit].setTilePosition(unit->getTilePosition());
	}
	return;
}

void WorkerTrackerClass::removeWorker(Unit worker)
{
	if (Resources().getMyGas().find(myWorkers[worker].getResource()) != Resources().getMyGas().end())
	{
		Resources().getMyGas()[myWorkers[worker].getResource()].setGathererCount(Resources().getMyGas()[myWorkers[worker].getResource()].getGathererCount() - 1);
	}
	if (Resources().getMyMinerals().find(myWorkers[worker].getResource()) != Resources().getMyMinerals().end())
	{
		Resources().getMyMinerals()[myWorkers[worker].getResource()].setGathererCount(Resources().getMyMinerals()[myWorkers[worker].getResource()].getGathererCount() - 1);
	}
	myWorkers.erase(worker);
}

void WorkerTrackerClass::assignWorker(WorkerInfo& worker)
{
	// Assign a task if none
	int cnt = 1;

	if (!Strategy().isRush() || Resources().isMinSaturated())
	{
		for (auto &gas : Resources().getMyGas())
		{
			if (gas.second.getType() != UnitTypes::Resource_Vespene_Geyser && gas.first->isCompleted() && gas.second.getGathererCount() < 3)
			{
				gas.second.setGathererCount(gas.second.getGathererCount() + 1);
				worker.setResource(gas.first);
				worker.setResourcePosition(gas.second.getPosition());
				return;
			}
		}
	}

	// First checks if a mineral field has 0 workers mining, if none, checks if a mineral field has 1 worker mining. Assigns to 0 first, then 1. Spreads saturation.
	while (cnt <= 2)
	{
		for (auto &mineral : Resources().getMyMinerals())
		{
			if (mineral.second.getGathererCount() < cnt)
			{
				mineral.second.setGathererCount(mineral.second.getGathererCount() + 1);
				worker.setResource(mineral.first);
				worker.setResourcePosition(mineral.second.getPosition());
				return;
			}
		}
		cnt++;
	}
	return;
}

void WorkerTrackerClass::reAssignWorker(WorkerInfo& worker)
{
	if (worker.getResource())
	{
		if (Resources().getMyGas().find(worker.getResource()) != Resources().getMyGas().end())
		{
			Resources().getMyGas()[worker.getResource()].setGathererCount(Resources().getMyGas()[worker.getResource()].getGathererCount() - 1);
		}
		if (Resources().getMyMinerals().find(worker.getResource()) != Resources().getMyMinerals().end())
		{
			Resources().getMyMinerals()[worker.getResource()].setGathererCount(Resources().getMyMinerals()[worker.getResource()].getGathererCount() - 1);
		}
	}
	assignWorker(worker);
}