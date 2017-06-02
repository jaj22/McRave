#include "ProbeManager.h"
#include "ResourceManager.h"
#include "TerrainManager.h"
#include "GridManager.h"
#include "UnitManager.h"
#include "StrategyManager.h"
#include "UnitUtil.h"
#include "CommandManager.h"

using namespace BWAPI;
using namespace std;

void ProbeTrackerClass::update()
{
	scoutProbe();
	enforceAssignments();
}

void ProbeTrackerClass::storeProbe(Unit unit)
{
	if (unit->exists() && unit->isCompleted())
	{
		myProbes[unit].setMiniTile(UnitUtil::Instance().getMiniTile(unit));
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
	if (probe == scout && isScouting())
	{
		scouting = false;
		if (TerrainTracker::Instance().getEnemyBasePositions().size() == 0)
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
			TerrainTracker::Instance().getEnemyBasePositions().emplace(closestB->getPosition());
		}
	}
}

void ProbeTrackerClass::assignProbe(Unit probe)
{
	// Assign a task if none
	int cnt = 1;

	for (auto &gas : ResourceTracker::Instance().getMyGas())
	{
		if (gas.second.getType() == UnitTypes::Protoss_Assimilator && gas.first->isCompleted() && gas.second.getGathererCount() < 3)
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

void ProbeTrackerClass::reAssignProbe(Unit probe)
{
	if (ResourceTracker::Instance().getMyGas().find(myProbes[probe].getTarget()) != ResourceTracker::Instance().getMyGas().end())
	{
		ResourceTracker::Instance().getMyGas()[myProbes[probe].getTarget()].setGathererCount(ResourceTracker::Instance().getMyGas()[myProbes[probe].getTarget()].getGathererCount() - 1);
	}
	if (ResourceTracker::Instance().getMyMinerals().find(myProbes[probe].getTarget()) != ResourceTracker::Instance().getMyMinerals().end())
	{
		ResourceTracker::Instance().getMyMinerals()[myProbes[probe].getTarget()].setGathererCount(ResourceTracker::Instance().getMyMinerals()[myProbes[probe].getTarget()].getGathererCount() - 1);
	}

	assignProbe(probe);
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
			if (TerrainTracker::Instance().getEnemyBasePositions().size() == 0 && UnitTracker::Instance().getSupply() >= 18 && scouting)
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
			if (TerrainTracker::Instance().getEnemyBasePositions().size() > 0)
			{
				WalkPosition start = u.second.getMiniTile();
				if (u.first->getUnitsInRadius(320, Filter::IsEnemy && !Filter::IsBuilding).size() > 0)
				{
					avoidEnemy(u.first, u.first->getClosestUnit(Filter::IsEnemy));
				}
				else
				{
					exploreArea(u.first);
				}
			}
		}
	}
}

void ProbeTrackerClass::exploreArea(Unit probe)
{
	WalkPosition start = myProbes[probe].getMiniTile();
	vector<WalkPosition> possibilites;
	double closestD = 1000;
	recentExplorations[start] = Broodwar->getFrameCount();

	for (int x = start.x - 50; x < start.x + 50 + probe->getType().tileWidth(); x++)
	{
		for (int y = start.y - 50; y < start.y + 50 + probe->getType().tileHeight(); y++)
		{
			if (GridTracker::Instance().getAntiMobilityGrid(x, y) == 0 && GridTracker::Instance().getMobilityGrid(x, y) > 0 && GridTracker::Instance().getEGroundGrid(x, y) == 0.0 && GridTracker::Instance().getEDistanceGrid(x, y) == 0.0 && WalkPosition(x, y).isValid() && Broodwar->getFrameCount() - recentExplorations[WalkPosition(x, y)] > 200 && Position(WalkPosition(x, y)).getDistance(TerrainTracker::Instance().getEnemyStartingPosition()) < 320)
			{
				possibilites.push_back(WalkPosition(x, y));
			}
		}
	}

	if (possibilites.size() > 0)
	{
		int i = rand() % possibilites.size();
		probe->move(Position(possibilites.at(i)));
	}
	else
	{
		probe->move(TerrainTracker::Instance().getEnemyStartingPosition());
	}
}

void ProbeTrackerClass::enforceAssignments()
{
	// For each Probe mapped to gather minerals
	for (auto &u : myProbes)
	{
		if (ResourceTracker::Instance().getGasNeeded() > 0)
		{
			reAssignProbe(u.first);
			ResourceTracker::Instance().setGasNeeded(ResourceTracker::Instance().getGasNeeded() - 1);
		}
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
			if (u.first->getLastCommand().getType() == UnitCommandTypes::Attack_Unit)
			{
				u.first->attack(u.first->getClosestUnit(Filter::IsEnemy, 320));
			}			
			continue;
		}		

		// Else command probe
		if (u.first && u.first->exists())
		{
			// Draw on every frame
			if (u.first && u.second.getTarget())
			{
				Broodwar->drawLineMap(u.first->getPosition(), u.second.getTarget()->getPosition(), Broodwar->self()->getColor());
			}
			
			// If idle and carrying gas or minerals, return cargo			
			if (u.first->isCarryingGas() || u.first->isCarryingMinerals())
			{
				if (u.first->getLastCommand().getType() == UnitCommandTypes::Return_Cargo)
				{
					continue;
				}
				u.first->returnCargo();
				continue;
			}

			// If not scouting and there's boulders to remove	
			if (!scouting && ResourceTracker::Instance().getMyBoulders().size() > 0 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) >= 2)
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
			if (!u.first->isIdle() && (u.first->getLastCommand().getType() == UnitCommandTypes::Move || u.first->getLastCommand().getType() == UnitCommandTypes::Build))
			{
				continue;
			}


			// If not targeting the mineral field the Probe is mapped to
			if (!u.first->isCarryingGas() && !u.first->isCarryingMinerals())
			{
				if ((u.first->isGatheringMinerals() || u.first->isGatheringGas()) && u.first->getTarget() == u.second.getTarget())
				{
					continue;
				}
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

void ProbeTrackerClass::avoidEnemy(Unit probe, Unit target)
{
	// If either the unit or current target are invalid, return
	if (!probe || !target)
	{
		return;
	}

	WalkPosition start = ProbeTracker::Instance().getMyProbes()[probe].getMiniTile();
	WalkPosition finalPosition = start;
	double highestMobility = 0.0;
	Position destination;

	// Destination is enemy starting position, need to get as close to it as possible
	if (TerrainTracker::Instance().getEnemyStartingPosition().isValid())
	{
		destination = TerrainTracker::Instance().getEnemyStartingPosition();
	}
	// If it's not valid, check each starting position for where we were moving towards
	else
	{
		for (auto start : getStartLocations())
		{
			if (Broodwar->isExplored(start->getTilePosition()) == false)
			{
				destination = start->getPosition();
				break;
			}
		}
		// If still no destination, return
		if (!destination.isValid())
		{
			return;
		}
	}

	// Search a 16x16 (4 tiles) mini tile area around the unit for highest mobility	and lowest threat
	for (int x = start.x - 4; x <= start.x + 4 + (probe->getType().tileWidth() * 4); x++)
	{
		for (int y = start.y - 4; y <= start.y + 4 + (probe->getType().tileHeight() * 4); y++)
		{
			if (WalkPosition(x, y).isValid())
			{
				double mobility = double(GridTracker::Instance().getMobilityGrid(x, y));
				double threat = GridTracker::Instance().getEGroundGrid(x, y);
				double distance = GridTracker::Instance().getEDistanceGrid(x, y);
				double distanceEnemy = 1.0 + Position(start).getDistance(destination);

				if (GridTracker::Instance().getAntiMobilityGrid(x, y) == 0 && (mobility / (1.0 + (distance * threat))) / distanceEnemy > highestMobility && (getRegion(TilePosition(x / 4, y / 4)) && getRegion(probe->getTilePosition()) && getRegion(TilePosition(x / 4, y / 4)) == getRegion(probe->getTilePosition()) || (getNearestChokepoint(TilePosition(x / 4, y / 4)) && Position(x * 8, y * 8).getDistance(getNearestChokepoint(TilePosition(x / 4, y / 4))->getCenter()) < 128)))
				{
					bool bestTile = true;
					for (int i = x - probe->getType().width() / 16; i < x + probe->getType().width() / 16; i++)
					{
						for (int j = y - probe->getType().height() / 16; j < y + probe->getType().height() / 16; j++)
						{
							if (WalkPosition(i, j).isValid())
							{
								// If mini tile exists on top of unit, ignore it
								if (i >= start.x && i < start.x + probe->getType().tileWidth() * 4 && j >= start.y && j < start.y + probe->getType().tileHeight() * 4)
								{
									continue;
								}
								if (GridTracker::Instance().getMobilityGrid(i, j) == 0 || GridTracker::Instance().getAntiMobilityGrid(i, j) == 1)
								{
									bestTile = false;
								}
							}
						}
					}
					if (bestTile)
					{
						highestMobility = (mobility / (1.0 + (distance * threat))) / distanceEnemy;
						finalPosition = WalkPosition(x, y);
					}
				}
			}
		}
	}
	if (finalPosition.isValid() && finalPosition != start)
	{
		if (probe->getOrderTargetPosition() != Position(finalPosition))
		{
			probe->move(Position(finalPosition));
		}
	}
	return;
}