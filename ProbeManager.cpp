#include "McRave.h"

void ProbeTrackerClass::update()
{
	scoutProbe();
	enforceAssignments();
}

void ProbeTrackerClass::storeProbe(Unit unit)
{
	if (unit->exists() && unit->isCompleted())
	{
		myProbes[unit].setMiniTile(Util().getMiniTile(unit));
	}
	return;
}

void ProbeTrackerClass::removeProbe(Unit probe)
{
	if (Resources().getMyGas().find(myProbes[probe].getResource()) != Resources().getMyGas().end())
	{
		Resources().getMyGas()[myProbes[probe].getResource()].setGathererCount(Resources().getMyGas()[myProbes[probe].getResource()].getGathererCount() - 1);
	}
	if (Resources().getMyMinerals().find(myProbes[probe].getResource()) != Resources().getMyMinerals().end())
	{
		Resources().getMyMinerals()[myProbes[probe].getResource()].setGathererCount(Resources().getMyMinerals()[myProbes[probe].getResource()].getGathererCount() - 1);
	}
	myProbes.erase(probe);

	// If scouting probe died, assume where enemy is based on death location
	if (probe == scout && isScouting())
	{
		scout = nullptr;
		scouting = false;
		if (Terrain().getEnemyBasePositions().size() == 0)
		{
			double closestD = 0.0;
			BaseLocation* closestB = getNearestBaseLocation(probe->getTilePosition());
			for (auto base : getStartLocations())
			{
				if (base == getStartLocation(Broodwar->self()))
				{
					continue;
				}
				if (probe->getDistance(base->getPosition()) < closestD || closestD == 0.0)
				{
					closestB = base;
				}
			}
			Terrain().getEnemyBasePositions().emplace(closestB->getPosition());
		}
	}
}

void ProbeTrackerClass::assignProbe(Unit probe)
{
	// Assign a task if none
	int cnt = 1;

	for (auto &gas : Resources().getMyGas())
	{
		if (gas.second.getType() == UnitTypes::Protoss_Assimilator && gas.first->isCompleted() && gas.second.getGathererCount() < 3)
		{
			gas.second.setGathererCount(gas.second.getGathererCount() + 1);
			myProbes[probe].setResource(gas.first);
			myProbes[probe].setResourcePosition(Resources().getMyGas()[gas.first].getPosition());
			return;
		}
	}

	// First checks if a mineral field has 0 Probes mining, if none, checks if a mineral field has 1 Probe mining. Assigns to 0 first, then 1. Spreads saturation.
	while (cnt <= 2)
	{
		for (auto &mineral : Resources().getMyMinerals())
		{
			if (mineral.second.getGathererCount() < cnt)
			{
				mineral.second.setGathererCount(mineral.second.getGathererCount() + 1);
				myProbes[probe].setResource(mineral.first);
				myProbes[probe].setResourcePosition(Resources().getMyMinerals()[mineral.first].getPosition());
				return;
			}
		}
		cnt++;
	}
	return;
}

void ProbeTrackerClass::reAssignProbe(Unit probe)
{
	if (Resources().getMyGas().find(myProbes[probe].getResource()) != Resources().getMyGas().end())
	{
		Resources().getMyGas()[myProbes[probe].getResource()].setGathererCount(Resources().getMyGas()[myProbes[probe].getResource()].getGathererCount() - 1);
	}
	if (Resources().getMyMinerals().find(myProbes[probe].getResource()) != Resources().getMyMinerals().end())
	{
		Resources().getMyMinerals()[myProbes[probe].getResource()].setGathererCount(Resources().getMyMinerals()[myProbes[probe].getResource()].getGathererCount() - 1);
	}
	assignProbe(probe);
}

void ProbeTrackerClass::scoutProbe()
{
	if (!scouting)
	{
		return;
	}
	for (auto &u : myProbes)
	{
		// Crappy scouting method
		if (!scout)
		{
			scout = u.first;
		}
		if (u.first == scout)
		{
			if (Terrain().getEnemyBasePositions().size() == 0 && Units().getSupply() >= 18 && scouting)
			{
				for (auto start : getStartLocations())
				{
					if (Broodwar->isExplored(start->getTilePosition()) == false)
					{
						if (u.first->getOrderTargetPosition() != start->getPosition())
						{
							u.first->move(start->getPosition());
						}
						break;
					}
				}
			}
			if (Terrain().getEnemyBasePositions().size() > 0)
			{
				exploreArea(u.first);
			}
		}
	}
}

void ProbeTrackerClass::exploreArea(Unit probe)
{
	WalkPosition start = myProbes[probe].getMiniTile();
	Position bestPosition = Terrain().getEnemyStartingPosition();
	double closestD = 1000;
	recentExplorations[start] = Broodwar->getFrameCount();

	for (int x = start.x - 10; x < start.x + 10 + probe->getType().tileWidth(); x++)
	{
		for (int y = start.y - 10; y < start.y + 10 + probe->getType().tileHeight(); y++)
		{
			if (Grids().getAntiMobilityGrid(x, y) == 0 && Grids().getMobilityGrid(x, y) > 0 && Grids().getEGroundDistanceGrid(x, y) == 0.0 && WalkPosition(x, y).isValid() && Broodwar->getFrameCount() - recentExplorations[WalkPosition(x, y)] > 500 && Position(WalkPosition(x, y)).getDistance(Terrain().getEnemyStartingPosition()) < 320)
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
		probe->move(bestPosition);
		Broodwar->drawLineMap(probe->getPosition(), bestPosition, Colors::Red);
	}
	return;
}

void ProbeTrackerClass::enforceAssignments()
{
	// For each Probe mapped to gather minerals
	for (auto &u : myProbes)
	{
		if (Resources().getGasNeeded() > 0 && !Strategy().isRush())
		{
			reAssignProbe(u.first);
			Resources().setGasNeeded(Resources().getGasNeeded() - 1);
		}
		// If no valid target, try to get a new one
		if (!u.second.getResource())
		{
			assignProbe(u.first);
			// Any idle Probes can gather closest mineral field until they are assigned again
			if (u.first->isIdle() && u.first->getClosestUnit(Filter::IsMineralField))
			{
				u.first->gather(u.first->getClosestUnit(Filter::IsMineralField));
				continue;
			}
			continue;
		}

		// Attack units in mineral line
		if (Broodwar->getFrameCount() - u.second.getLastGatherFrame() <= 25 && Grids().getEGroundGrid(u.second.getMiniTile().x, u.second.getMiniTile().y) > 0)
		{
			if (u.second.getTarget() == nullptr)
			{
				u.second.setTarget(u.first->getClosestUnit(Filter::IsEnemy && !Filter::IsFlying, 320));
			}
			else if (u.second.getTarget()->exists() && (Grids().getResourceGrid(u.second.getTarget()->getTilePosition().x, u.second.getTarget()->getTilePosition().y) > 0/* || (!u.second.getTarget()->getType().isWorker() && Grids().getNexusGrid(u.second.getTarget()->getTilePosition().x, u.second.getTarget()->getTilePosition().y) > 0)*/))
			{
				if ((u.first->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && u.first->getLastCommand().getTarget() && !u.first->getLastCommand().getTarget()->exists()) || (u.first->getLastCommand().getType() != UnitCommandTypes::Attack_Unit))
				{
					u.first->attack(u.second.getTarget());
				}
				continue;
			}
		}

		// Else command probe
		if (u.first && u.first->exists())
		{
			// Draw on every frame
			if (u.first && u.second.getResource())
			{
				Broodwar->drawLineMap(u.first->getPosition(), u.second.getResourcePosition(), Broodwar->self()->getColor());
			}

			// If we have been given a command this frame already, continue
			if (!u.first->isIdle() && (u.first->getLastCommand().getType() == UnitCommandTypes::Move || u.first->getLastCommand().getType() == UnitCommandTypes::Build))
			{
				continue;
			}

			// If idle and carrying gas or minerals, return cargo			
			if (u.first->isCarryingGas() || u.first->isCarryingMinerals())
			{
				if (u.first->getLastCommand().getType() != UnitCommandTypes::Return_Cargo)
				{
					u.first->returnCargo();
				}
				continue;
			}

			// If not scouting and there's boulders to remove	
			if (!scouting && Resources().getMyBoulders().size() > 0 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) >= 2)
			{
				for (auto b : Resources().getMyBoulders())
				{
					if (b.first && b.first->exists() && !u.first->isGatheringMinerals() && u.first->getDistance(b.second.getPosition()) < 512)
					{
						u.first->gather(b.first);
						break;
					}
				}
			}

			// If not targeting the mineral field the Probe is mapped to
			if (!u.first->isCarryingGas() && !u.first->isCarryingMinerals())
			{
				if ((u.first->isGatheringMinerals() || u.first->isGatheringGas()) && u.first->getTarget() == u.second.getResource())
				{
					u.second.setLastGatherFrame(Broodwar->getFrameCount());
					continue;
				}
				// If the probes current target has a resource count of 0 (mineral blocking a ramp), let Probe continue mining it
				if (u.first->getTarget() && u.first->getTarget()->getType().isMineralField() && u.first->getTarget()->getResources() == 0)
				{
					continue;
				}
				// If the mineral field is in vision and no target, force to gather from the assigned mineral field
				if (u.second.getResource())
				{
					if (u.second.getResource()->exists())
					{
						u.first->gather(u.second.getResource());
						u.second.setLastGatherFrame(Broodwar->getFrameCount());
						continue;
					}
					else
					{
						u.first->move(u.second.getResourcePosition());
						continue;
					}
				}
			}
		}
	}
}

Unit ProbeTrackerClass::getClosestProbe(Position here)
{
	// Currently gets the closest probe that doesn't mine gas
	Unit closestProbe = nullptr;
	double closestD = 0.0;
	for (auto probe : myProbes)
	{
		if (probe.second.getResource() && probe.second.getResource()->exists() && !probe.second.getResource()->getType().isMineralField())
		{
			continue;
		}
		if (probe.first->getLastCommand().getType() == UnitCommandTypes::Move || probe.first->getLastCommand().getType() == UnitCommandTypes::Build)
		{
			continue;
		}
		if (probe.first->getLastCommand().getType() == UnitCommandTypes::Gather && probe.first->getLastCommand().getTarget()->exists() && probe.first->getLastCommand().getTarget()->getInitialResources() == 0)
		{
			continue;
		}
		if (probe.first->getDistance(here) < closestD || closestD == 0.0)
		{
			closestProbe = probe.first;
			closestD = probe.first->getDistance(here);
		}
	}
	return closestProbe;
}