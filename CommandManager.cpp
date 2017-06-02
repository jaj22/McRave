#include "CommandManager.h"
#include "GridManager.h"
#include "StrategyManager.h"
#include "TerrainManager.h"
#include "UnitManager.h"
#include "NexusManager.h"
#include "BuildOrder.h"
#include "UnitUtil.h"
#include "ProbeManager.h"

void CommandTrackerClass::update()
{
	for (auto &u : UnitTracker::Instance().getMyUnits())
	{
		getDecision(u.first, u.second.getTarget());
	}
}

void CommandTrackerClass::getDecision(Unit unit, Unit target)
{
	int offset = 0;
	double closestD = 0.0;
	Position closestP;

	// Stop offset required for units with animations
	if (unit->getType() == UnitTypes::Protoss_Dragoon)
	{
		offset = 9;
	}

	// Ignore the unit if it no longer exists, is locked down, maelstrommed, stassised, not powered or not completed
	if (!unit || !unit->exists() || unit->isLockedDown() || unit->isMaelstrommed() || unit->isStasised() || !unit->isCompleted())
	{
		return;
	}

	// If Reaver, train scarabs
	if (unit->getType() == UnitTypes::Protoss_Reaver && unit->getScarabCount() < 5)
	{
		unit->train(UnitTypes::Protoss_Scarab);
	}

	// If the unit is ready to perform an action after an attack (Dragoons require 9 frames after an attack)
	if (Broodwar->getFrameCount() - UnitTracker::Instance().getMyUnits()[unit].getLastAttackFrame() > offset - Broodwar->getLatencyFrames())
	{
		if ((unit->getLastCommand().getType() == UnitCommandTypes::Right_Click_Unit && unit->getShields() < 40) || unit->getShields() < 10 && unit->getUnitsInRadius(320, Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Shield_Battery).size() > 0)
		{
			if (unit->getLastCommand().getType() != UnitCommandTypes::Right_Click_Unit)
			{
				unit->rightClick(unit->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Shield_Battery));
			}
			return;
		}
		// If globally behind
		if (UnitTracker::Instance().getGlobalStrategy() == 0)
		{
			// Check if we have a target
			if (target && target != nullptr)
			{
				// If locally ahead, fight
				if (UnitTracker::Instance().getMyUnits()[unit].getStrategy() == 1)
				{
					microTarget(unit, target);
					return;
				}
				// Else flee into ally territory (hopefully)
				else if (UnitTracker::Instance().getMyUnits()[unit].getStrategy() == 0)
				{
					fleeTarget(unit, target);
					return;
				}
			}

			// Else defend
			if (TerrainTracker::Instance().getEnemyBasePositions().size() > 0 && TerrainTracker::Instance().getAllyTerritory().size() > 0)
			{
				defend(unit, target);
				return;
			}
		}

		// If globally ahead
		else if (UnitTracker::Instance().getGlobalStrategy() == 1 && TerrainTracker::Instance().getEnemyBasePositions().size() > 0)
		{
			// Check if we have a target
			if (target && target != nullptr)
			{
				// If locally behind, contain
				if (UnitTracker::Instance().getMyUnits()[unit].getStrategy() == 0)
				{
					fleeTarget(unit, target);
					return;
				}
				// Else attack
				else if (target->exists())
				{
					microTarget(unit, target);
					return;
				}
			}

			// If target doesn't exist, attack move towards it
			if (target && UnitTracker::Instance().getEnUnits()[target].getPosition().isValid())
			{
				if (unit->getOrderTargetPosition() != UnitTracker::Instance().getEnUnits()[target].getPosition())
				{
					unit->attack(UnitTracker::Instance().getEnUnits()[target].getPosition());
				}
				return;
			}

			// If no target, attack closest enemy expansion
			for (auto base : TerrainTracker::Instance().getEnemyBasePositions())
			{
				if (unit->getDistance(base) < closestD || closestD == 0.0)
				{
					closestP = base;
					closestD = unit->getDistance(base);
				}
			}
			if (unit->getOrderTargetPosition() != closestP)
			{
				unit->attack(closestP);
			}
			return;
		}
	}
	return;
}

void CommandTrackerClass::microTarget(Unit unit, Unit target)
{
	// Variables
	bool kite = false;
	int range = (int)UnitTracker::Instance().getMyUnits()[unit].getGroundRange();

	
	// If kiting unnecessary, disable
	if (target->getType().isBuilding() || unit->getType() == UnitTypes::Protoss_Corsair)
	{
		kite = false;
	}

	// Reavers should always kite away from their target if it has lower range
	else if (unit->getType() == UnitTypes::Protoss_Reaver && UnitTracker::Instance().getEnUnits()[target].getGroundRange() < range)
	{
		kite = true;
	}

	// If kiting is a good idea, enable
	else if (target->getType() == UnitTypes::Terran_Vulture_Spider_Mine || (range > 32 && unit->isUnderAttack()) || (UnitTracker::Instance().getEnUnits()[target].getGroundRange() <= range && (unit->getDistance(target) <= range - UnitTracker::Instance().getEnUnits()[target].getGroundRange() && UnitTracker::Instance().getEnUnits()[target].getGroundRange() > 0 && range > 32 || unit->getHitPoints() < 40)))
	{
		kite = true;
	}

	// If kite is true and weapon on cooldown, move
	if (kite && unit->getGroundWeaponCooldown() > 0)
	{
		fleeTarget(unit, target);
	}
	else if (unit->getGroundWeaponCooldown() <= 0)
	{
		// If unit receieved an attack command on the target already, don't give another order - TODO: Test if it could be removed maybe to prevent goon stop bug
		if (unit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit->getLastCommand().getTarget() == target)
		{
			return;
		}
		if (unit->getOrderTarget() != target)
		{
			unit->attack(target);
		}
		UnitTracker::Instance().getMyUnits()[unit].setTargetPosition(target->getPosition());
	}
	return;
}

void CommandTrackerClass::exploreArea(Unit unit)
{
	// Given a region, explore a random portion of it based on random metrics like:
	// Distance to enemy
	// Distance to home
	// Last explored
	// Unit deaths
	// Untouched resources
}

void CommandTrackerClass::fleeTarget(Unit unit, Unit target)
{
	// If either the unit or current target are invalid, return
	if (!unit || !target)
	{
		return;
	}

	WalkPosition start;
	if (unit->getType().isWorker())
	{
		start = ProbeTracker::Instance().getMyProbes()[unit].getMiniTile();
	}
	else
	{
		start = UnitTracker::Instance().getMyUnits()[unit].getMiniTile();
	}

	WalkPosition finalPosition = start;
	double highestMobility = 0.0;

	// Search a 16x16 (4 tiles) mini tile area around the unit for highest mobility	and lowest threat
	for (int x = start.x - 12; x <= start.x + 12 + (unit->getType().tileWidth() * 4); x++)
	{
		for (int y = start.y - 12; y <= start.y + 12 + (unit->getType().tileHeight() * 4); y++)
		{
			if (WalkPosition(x, y).isValid())
			{		
				if (unit->getType() == UnitTypes::Protoss_Dragoon && GridTracker::Instance().getResourceGrid(x / 4, y / 4) > 0)
				{
					continue;
				}

				double mobility = double(GridTracker::Instance().getMobilityGrid(x, y));
				double threat = GridTracker::Instance().getEGroundGrid(x, y);
				double distance = GridTracker::Instance().getEDistanceGrid(x, y);
				double distanceHome = double(pow(GridTracker::Instance().getDistanceHome(x, y), 0.1));
				
				if (GridTracker::Instance().getAntiMobilityGrid(x, y) == 0 && (mobility / (1.0 + (distance * threat))) / distanceHome > highestMobility && (getRegion(TilePosition(x / 4, y / 4)) && getRegion(unit->getTilePosition()) && getRegion(TilePosition(x / 4, y / 4)) == getRegion(unit->getTilePosition()) || (getNearestChokepoint(TilePosition(x / 4, y / 4)) && Position(x * 8, y * 8).getDistance(getNearestChokepoint(TilePosition(x / 4, y / 4))->getCenter()) < 128)))
				{
					bool bestTile = true;
					for (int i = x - unit->getType().width() / 16; i < x + unit->getType().width() / 16; i++)
					{
						for (int j = y - unit->getType().height() / 16; j < y + unit->getType().height() / 16; j++)
						{
							if (WalkPosition(i, j).isValid())
							{
								// If mini tile exists on top of unit, ignore it
								if (i >= start.x && i < start.x + unit->getType().tileWidth() * 4 && j >= start.y && j < start.y + unit->getType().tileHeight() * 4)
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
						highestMobility = (mobility / (1.0 + (distance * threat))) / distanceHome;
						finalPosition = WalkPosition(x, y);
					}
				}
			}
		}
	}
	if (finalPosition.isValid() && finalPosition != start)
	{
		if (!unit->getType().isWorker())
		{
			GridTracker::Instance().updateAllyMovement(unit, finalPosition);
			UnitTracker::Instance().getMyUnits()[unit].setTargetPosition(Position(finalPosition));
		}
		if (unit->getOrderTargetPosition() != Position(finalPosition))
		{
			unit->move(Position(finalPosition));			
		}
	}
	return;
}

void CommandTrackerClass::defend(Unit unit, Unit target)
{
	double closestD = 0.0;
	Position closestP;

	// If expanding to natural, hold at natural choke
	if (BuildOrderTracker::Instance().getBuildingDesired()[Protoss_Nexus] > Broodwar->self()->visibleUnitCount(Protoss_Nexus))
	{
		closestP = Position(TerrainTracker::Instance().getPath().at(1)->Center());
		if (TerrainTracker::Instance().getAllyTerritory().find(getNearestChokepoint(TilePosition(TerrainTracker::Instance().getPath().at(1)->Center()))->getRegions().second) != TerrainTracker::Instance().getAllyTerritory().end() || TerrainTracker::Instance().getAllyTerritory().find(getNearestChokepoint(TilePosition(TerrainTracker::Instance().getPath().at(1)->Center()))->getRegions().first) != TerrainTracker::Instance().getAllyTerritory().end())
		{
			closestP = Position(TerrainTracker::Instance().getPath().at(2)->Center());
		}
		if (unit->getOrderTargetPosition() != closestP)
		{
			unit->move(closestP);
		}
		return;
	}

	// Move from choke if enemies nearby

	// Early on, defend mineral line
	if (Broodwar->getFrameCount() < 6000)
	{
		for (auto nexus : NexusTracker::Instance().getMyNexus())
		{
			if (unit->getOrderTargetPosition() != (Position(nexus.second.getCannonPosition()) + Position(nexus.first->getPosition())) / 2)
			{
				unit->move((Position(nexus.second.getCannonPosition()) + Position(nexus.first->getPosition())) / 2);
			}
			return;
		}
	}

	// Defend closest chokepoint 
	for (auto position : TerrainTracker::Instance().getDefendHere())
	{
		if (unit->getDistance(position) < 128)
		{
			closestP = position;
			break;
		}
		else if (unit->getDistance(position) <= closestD || closestD == 0)
		{
			closestD = unit->getDistance(position);
			closestP = position;
		}
	}
	if (unit->getOrderTargetPosition() != closestP)
	{
		unit->move(closestP);
	}
	return;
}