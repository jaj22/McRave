#include "ProbeManager.h"
#include "ResourceManager.h"
#include "TerrainManager.h"
#include "GridManager.h"
#include "UnitManager.h"
#include "StrategyManager.h"

using namespace BWAPI;
using namespace std;

void ProbeTrackerClass::update()
{
	storeProbes();
	enforceAssignments();
	scoutProbe();
}

void ProbeTrackerClass::storeProbes()
{
	for (auto probe : Broodwar->self()->getUnits())
	{
		if (probe->exists() && probe->isCompleted() && probe->getType() == UnitTypes::Protoss_Probe && myProbes.find(probe) == myProbes.end())
		{
			ProbeInfo newProbe;
			myProbes[probe] = newProbe;
		}
	}
	return;
}

void ProbeTrackerClass::removeProbe(Unit probe)
{
	if (ResourceTracker::Instance().getMyGas().find(myProbes[probe].getTarget()) != ResourceTracker::Instance().getMyGas().end())
	{
		ResourceTracker::Instance().getMyGas()[myProbes[probe].getTarget()].setGathererCount(ResourceTracker::Instance().getMyGas()[myProbes[probe].getTarget()].getGathererCount() - 1);
	}
	if (ResourceTracker::Instance().getMyMinerals().find(myProbes[probe].getTarget()) != ResourceTracker::Instance().getMyMinerals().end())
	{
		ResourceTracker::Instance().getMyMinerals()[myProbes[probe].getTarget()].setGathererCount(ResourceTracker::Instance().getMyMinerals()[myProbes[probe].getTarget()].getGathererCount() - 1);
	}
	myProbes.erase(probe);
	
	// If scouting probe died, assume where enemy is based on death location
	if (probe == scout && isScouting() && TerrainTracker::Instance().getEnemyBasePositions().size() == 0)
	{
		double closestD = 0.0;
		BaseLocation* closestB = getNearestBaseLocation(probe->getTilePosition());
		for (auto base : getStartLocations())
		{
			if (probe->getDistance(base->getPosition()) < closestD || closestD == 0.0)
			{
				closestB = base;
			}
		}
	}
}

void ProbeTrackerClass::assignProbe(Unit probe)
{
	// Assign a task if none
	int cnt = 1;

	for (auto &gas : ResourceTracker::Instance().getMyGas())
	{
		if (gas.second.getUnitType() == UnitTypes::Protoss_Assimilator && gas.first->isCompleted() && gas.second.getGathererCount() < 3)
		{
			gas.second.setGathererCount(gas.second.getGathererCount() + 1);
			myProbes[probe].setTarget(gas.first);
			return;
		}
	}

	// First checks if a mineral field has 0 Probes mining, if none, checks if a mineral field has 1 Probe mining. Assigns to 0 first, then 1. Spreads saturation.
	while (cnt <= 2)
	{
		for (auto &mineral : ResourceTracker::Instance().getMyMinerals())
		{
			if (mineral.second.getGathererCount() < cnt)
			{
				mineral.second.setGathererCount(mineral.second.getGathererCount() + 1);
				myProbes[probe].setTarget(mineral.first);
				return;
			}
		}
		cnt++;
	}
	return;
}

void ProbeTrackerClass::scoutProbe()
{
	for (auto &u : myProbes)
	{
		// Crappy scouting method
		if (!scout)
		{
			scout = u.first;
		}
		if (u.first == scout)
		{
			if (UnitTracker::Instance().getSupply() >= 18 && scouting)
			{
				for (auto start : getStartLocations())
				{
					if (Broodwar->isExplored(start->getTilePosition()) == false)
					{
						u.first->move(start->getPosition());
						break;
					}
				}
			}
			else if (u.first->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsWorker && Filter::CanAttack).size() > 0)
			{
				u.first->stop();
				scouting = false;
			}
		}
	}
}

void ProbeTrackerClass::enforceAssignments()
{
	// For each Probe mapped to gather minerals
	for (auto &u : myProbes)
	{
		// If no valid target, try to get a new one
		if (!u.second.getTarget())
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
		if (GridTracker::Instance().getResourceGrid(u.first->getTilePosition().x, u.first->getTilePosition().y) > 0 && u.first->getUnitsInRadius(64, Filter::IsEnemy).size() > 0 && (u.first->getHitPoints() + u.first->getShields()) > 20)
		{
			u.first->attack(u.first->getClosestUnit(Filter::IsEnemy, 320));
		}
		else if (u.first->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && (GridTracker::Instance().getResourceGrid(u.first->getTilePosition().x, u.first->getTilePosition().y) == 0 || (u.first->getHitPoints() + u.first->getShields()) <= 20))
		{
			u.first->stop();
		}

		// Else command probe
		if (u.first && u.first->exists())
		{
			// Draw on every frame
			if (u.first && u.second.getTarget())
			{
				Broodwar->drawLineMap(u.first->getPosition(), u.second.getTarget()->getPosition(), Broodwar->self()->getColor());
			}

			// Return if not latency frame
			if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
			{
				continue;
			}

			// If idle and carrying minerals, return cargo			
			if (u.first->isIdle() && u.first->isCarryingMinerals())
			{
				u.first->returnCargo();
				continue;
			}

			// If not scouting and there's boulders to remove
			if (!scouting && ResourceTracker::Instance().getMyBoulders().size() > 0)
			{
				for (auto b : ResourceTracker::Instance().getMyBoulders())
				{
					if (b.first && b.first->exists() && !u.first->isGatheringMinerals() && u.first->getDistance(b.second.getPosition()) < 512)
					{
						u.first->gather(b.first);
						break;
					}
				}
			}

			// If we have been given a command this frame already, continue
			if (u.first->getLastCommandFrame() >= Broodwar->getFrameCount() && (u.first->getLastCommand().getType() == UnitCommandTypes::Move || u.first->getLastCommand().getType() == UnitCommandTypes::Build))
			{
				continue;
			}


			// If idle and not targeting the mineral field the Probe is mapped to
			if (u.first->isIdle() || (u.first->isGatheringMinerals() && !u.first->isCarryingMinerals() && u.first->getTarget() != u.second.getTarget()))
			{
				// If the Probe has a target
				if (u.first->getTarget())
				{
					// If the target has a resource count of 0 (mineral blocking a ramp), let Probe continue mining it
					if (u.first->getTarget()->getResources() == 0)
					{
						continue;
					}
				}
				// If the mineral field is in vision and no target, force to gather from the assigned mineral field
				if (u.second.getTarget() && u.second.getTarget()->exists())
				{
					u.first->gather(u.second.getTarget());
					continue;
				}
			}
		}
	}
}