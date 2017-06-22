#include "McRave.h"

void ProbeTrackerClass::update()
{
	for (auto &probe : myProbes)
	{
		updateScout(probe.second);
		updateDecision(probe.second);
	}
}

void ProbeTrackerClass::updateScout(ProbeInfo& probe)
{
	if (!scouting)
	{
		return;
	}

	// Crappy scouting method
	if (!scout)
	{
		scout = probe.unit();
	}
	if (probe.unit() == scout)
	{
		if (Terrain().getEnemyBasePositions().size() == 0 && Units().getSupply() >= 18 && scouting)
		{
			for (auto start : getStartLocations())
			{
				if (Broodwar->isExplored(start->getTilePosition()) == false)
				{
					if (probe.unit()->getOrderTargetPosition() != start->getPosition())
					{
						probe.unit()->move(start->getPosition());
					}
					break;
				}
			}
		}
		if (Terrain().getEnemyBasePositions().size() > 0)
		{
			exploreArea(probe.unit());
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

void ProbeTrackerClass::updateDecision(ProbeInfo& probe)
{
	// Reassign probes if we need gas
	if (Resources().getGasNeeded() > 0 && !Strategy().isRush())
	{
		reAssignProbe(probe.unit());
		Resources().setGasNeeded(Resources().getGasNeeded() - 1);
	}

	// If probe has no shields left and on a shield battery grid
	if (probe.unit()->getShields() <= 5 && Grids().getBatteryGrid(probe.unit()->getTilePosition()) > 0)
	{
		if (probe.unit()->getLastCommand().getType() != UnitCommandTypes::Right_Click_Unit && probe.unit()->getClosestUnit(Filter::IsAlly && Filter::IsCompleted  && Filter::GetType == UnitTypes::Protoss_Shield_Battery && Filter::Energy > 10))
		{
			probe.unit()->rightClick(probe.unit()->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Shield_Battery));
			return;
		}		
	}

	// If no valid target, try to get a new one
	if (!probe.getResource())
	{
		assignProbe(probe.unit());
		// Any idle Probes can gather closest mineral field until they are assigned again
		if (probe.unit()->isIdle() && probe.unit()->getClosestUnit(Filter::IsMineralField))
		{
			probe.unit()->gather(probe.unit()->getClosestUnit(Filter::IsMineralField));
			probe.setTarget(nullptr);
			return;
		}
		return;
	}

	// Attack units in mineral line
	if (Broodwar->getFrameCount() - probe.getLastGatherFrame() <= 25 && Grids().getEGroundDistanceGrid(probe.getMiniTile().x, probe.getMiniTile().y) > 0)
	{
		if (!probe.getTarget() || (probe.getTarget() && !probe.getTarget()->exists()))
		{
			probe.setTarget(probe.unit()->getClosestUnit(Filter::IsEnemy && !Filter::IsFlying, 320));
		}
		else if (probe.getTarget()->exists() && (Grids().getResourceGrid(probe.getTarget()->getTilePosition().x, probe.getTarget()->getTilePosition().y) > 0/* || (!probe.getTarget()->getType().isWorker() && Grids().getNexusGrid(probe.getTarget()->getTilePosition().x, probe.getTarget()->getTilePosition().y) > 0)*/))
		{
			if ((probe.unit()->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && probe.unit()->getLastCommand().getTarget() && !probe.unit()->getLastCommand().getTarget()->exists()) || (probe.unit()->getLastCommand().getType() != UnitCommandTypes::Attack_Unit))
			{
				probe.unit()->attack(probe.getTarget());
			}
			return;
		}
	}

	// Else command probe
	if (probe.unit() && probe.unit()->exists())
	{
		// Draw on every frame
		if (probe.unit() && probe.getResource())
		{
			Broodwar->drawLineMap(probe.unit()->getPosition(), probe.getResourcePosition(), Broodwar->self()->getColor());
		}

		// If we have been given a command this frame already, return
		if (!probe.unit()->isIdle() && (probe.unit()->getLastCommand().getType() == UnitCommandTypes::Move || probe.unit()->getLastCommand().getType() == UnitCommandTypes::Build))
		{
			return;
		}

		// If idle and carrying gas or minerals, return cargo			
		if (probe.unit()->isCarryingGas() || probe.unit()->isCarryingMinerals())
		{
			if (probe.unit()->getLastCommand().getType() != UnitCommandTypes::Return_Cargo)
			{
				probe.unit()->returnCargo();
			}
			return;
		}

		// If not scouting and there's boulders to remove	
		if (!scouting && Resources().getMyBoulders().size() > 0 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) >= 2)
		{
			for (auto b : Resources().getMyBoulders())
			{
				if (b.first && b.first->exists() && !probe.unit()->isGatheringMinerals() && probe.unit()->getDistance(b.second.getPosition()) < 512)
				{
					probe.unit()->gather(b.first);
					break;
				}
			}
		}

		// If not targeting the mineral field the Probe is mapped to
		if (!probe.unit()->isCarryingGas() && !probe.unit()->isCarryingMinerals())
		{
			if ((probe.unit()->isGatheringMinerals() || probe.unit()->isGatheringGas()) && probe.unit()->getTarget() == probe.getResource())
			{				
				probe.setLastGatherFrame(Broodwar->getFrameCount());
				return;
			}
			// If the probes current target has a resource count of 0 (mineral blocking a ramp), let Probe continue mining it
			if (probe.unit()->getTarget() && probe.unit()->getTarget()->getType().isMineralField() && probe.unit()->getTarget()->getResources() == 0)
			{
				return;
			}
			// If the mineral field is in vision and no target, force to gather from the assigned mineral field
			if (probe.getResource())
			{
				if (probe.getResource()->exists())
				{
					probe.unit()->gather(probe.getResource());
					probe.setLastGatherFrame(Broodwar->getFrameCount());
					return;
				}
				else
				{
					probe.unit()->move(probe.getResourcePosition());
					return;
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

void ProbeTrackerClass::storeProbe(Unit unit)
{
	if (unit->exists() && unit->isCompleted())
	{
		myProbes[unit].setUnit(unit);
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

	if (!Strategy().isRush() || Resources().isMinSaturated())
	{
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