#include "CommandManager.h"
#include "GridManager.h"
#include "StrategyManager.h"
#include "TerrainManager.h"
#include "UnitManager.h"
#include "NexusManager.h"
#include "BuildOrder.h"

void CommandTrackerClass::update()
{
	for (auto &u : UnitTracker::Instance().getMyUnits())
	{
		getLocalCalculation(u.first, u.second.getTarget());
		getGlobalCalculation(u.first, u.second.getTarget());
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
		// If globally behind
		if (globalStrategy == 0)
		{
			// If locally ahead, fight
			if (UnitTracker::Instance().getMyUnits()[unit].getStrategy() == 1)
			{
				microTarget(unit, target);
				return;
			}
			// Else defend
			if (TerrainTracker::Instance().getEnemyBasePositions().size() > 0 && TerrainTracker::Instance().getAllyTerritory().size() > 0)
			{
				defend(unit, target);
				return;
			}			
		}

		// If globally ahead
		else if (globalStrategy == 1 && TerrainTracker::Instance().getEnemyBasePositions().size() > 0)
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
				else if (UnitTracker::Instance().getMyUnits()[unit].getStrategy() == 1 && target->exists())
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

void CommandTrackerClass::getLocalCalculation(Unit unit, Unit target)
{
	// Variables for calculating local strengths
	double enemyLocalStrength = 0.0, allyLocalStrength = 0.0, thisUnit = 0.0;
	Position targetPosition = UnitTracker::Instance().getEnUnits()[target].getPosition();
	int radius = min(512, 384 + UnitTracker::Instance().getSupply() * 4);

	int aLarge = UnitTracker::Instance().getMySizes()[UnitSizeTypes::Large];
	int aMedium = UnitTracker::Instance().getMySizes()[UnitSizeTypes::Medium];
	int aSmall = UnitTracker::Instance().getMySizes()[UnitSizeTypes::Small];

	int eLarge = UnitTracker::Instance().getEnSizes()[UnitSizeTypes::Large];
	int eMedium = UnitTracker::Instance().getEnSizes()[UnitSizeTypes::Medium];
	int eSmall = UnitTracker::Instance().getEnSizes()[UnitSizeTypes::Small];

	// Reset local
	UnitTracker::Instance().getMyUnits()[unit].setLocal(0);

	if (unit->getDistance(targetPosition) > 512)
	{
		UnitTracker::Instance().getMyUnits()[unit].setStrategy(3);
		return;
	}

	// Check every enemy unit being in range of the target
	for (auto &u : UnitTracker::Instance().getEnUnits())
	{
		// Ignore workers, keep buildings (reinforcements and static defenses)
		if (u.second.getType().isWorker() || (u.first && u.first->exists() && u.first->isStasised()))
		{
			continue;
		}

		// Reset unit strength
		thisUnit = 0.0;
		double threatRange = (u.second.getGroundRange() / 32.0) + u.second.getSpeed() + double(u.second.getType().width()) / 32 + double(UnitTracker::Instance().getEnUnits()[target].getType().width()) / 32;

		// If a unit is within range of the target, add to local strength
		if (u.second.getPosition().getDistance(targetPosition) / 32.0 < threatRange)
		{
			if (aLarge > 0 || aMedium > 0 || aSmall > 0)
			{
				// If unit is cloaked or burrowed and not detected, drastically increase strength
				if ((u.first->isCloaked() || u.first->isBurrowed()) && !u.first->isDetected())
				{
					thisUnit = 20 * u.second.getMaxStrength();
				}
				else if (u.first->getType().groundWeapon().damageType() == DamageTypes::Explosive)
				{
					thisUnit = u.second.getStrength() * ((aLarge*1.0) + (aMedium*0.75) + (aSmall*0.5)) / (aLarge + aMedium + aSmall);
				}
				else if (u.first->getType().groundWeapon().damageType() == DamageTypes::Concussive)
				{
					thisUnit = u.second.getStrength() * ((aLarge*1.0) + (aMedium*0.75) + (aSmall*0.5)) / (aLarge + aMedium + aSmall);
				}
				else
				{
					thisUnit = u.second.getStrength();
				}
			}
			else
			{
				thisUnit = u.second.getStrength();
			}
			// If enemy hasn't died, add to enemy. Otherwise, partially add to ally local
			if (u.second.getDeadFrame() == 0)
			{
				enemyLocalStrength += thisUnit;
			}
			else
			{
				allyLocalStrength += u.second.getMaxStrength() * 1.0 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
			}
		}
	}

	// Check every ally being in range of the target
	for (auto &u : UnitTracker::Instance().getMyUnits())
	{
		// Ignore workers and buildings
		if (u.second.getType().isWorker() || u.second.getType().isBuilding())
		{
			continue;
		}

		// Reset unit strength
		thisUnit = 0.0;
		double threatRange = (u.second.getGroundRange() / 32.0) + u.second.getSpeed() + double(u.second.getType().width()) / 32.0 + double(UnitTracker::Instance().getEnUnits()[target].getType().width()) / 32;

		// If a unit is within the range of the ally unit, add to local strength
		if (u.second.getPosition().getDistance(unit->getPosition()) / 32.0 < threatRange)
		{
			if (eLarge > 0 || eMedium > 0 || eSmall > 0)
			{
				// If shuttle, add units inside
				if (u.second.getType() == UnitTypes::Protoss_Shuttle && u.first->getLoadedUnits().size() > 0)
				{
					// Assume reaver for damage type calculations
					for (Unit uL : u.first->getLoadedUnits())
					{
						thisUnit = UnitTracker::Instance().getMyUnits()[uL].getStrength();
					}
				}
				else
				{
					// Damage type calculations
					if (u.second.getType().groundWeapon().damageType() == DamageTypes::Explosive)
					{
						thisUnit = u.second.getStrength() * ((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / (eLarge + eMedium + eSmall);
					}
					else if (u.second.getType().groundWeapon().damageType() == DamageTypes::Concussive)
					{
						thisUnit = u.second.getStrength() * ((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / (eLarge + eMedium + eSmall);
					}
					else
					{
						thisUnit = u.second.getStrength();
					}
				}
			}
			else
			{
				thisUnit = u.second.getStrength();
			}

			// If ally hasn't died, add to ally. Otherwise, partially add to enemy local
			if (u.second.getDeadFrame() == 0)
			{
				allyLocalStrength += thisUnit;
			}
			else
			{
				enemyLocalStrength += u.second.getMaxStrength() * 1.0 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
			}
		}
	}

	// Store the difference of strengths 
	UnitTracker::Instance().getMyUnits()[unit].setLocal(allyLocalStrength - enemyLocalStrength);

	// If we are in ally territory and have a target, force to fight	
	if (target && target->exists())
	{
		if (unit->getType() == UnitTypes::Protoss_Zealot && Broodwar->getFrameCount() < 3000)
		{
			if (GridTracker::Instance().getResourceGrid(target->getTilePosition().x, target->getTilePosition().y) > 0)
			{
				UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
				return;
			}
			else
			{
				UnitTracker::Instance().getMyUnits()[unit].setStrategy(0);
				return;
			}
		}
		if (Broodwar->getFrameCount() > 8000 && TerrainTracker::Instance().getAllyTerritory().find(getRegion(unit->getPosition())) != TerrainTracker::Instance().getAllyTerritory().end())
		{
			UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
			return;
		}
		if (UnitTracker::Instance().getMyUnits()[unit].getGroundRange() > UnitTracker::Instance().getEnUnits()[target].getGroundRange())
		{
			bool safeTile = true;
			for (auto miniTile : UnitTracker::Instance().getMiniTilesUnderUnit(unit))
			{
				if (miniTile.isValid() && GridTracker::Instance().getEDistanceGrid(miniTile.x, miniTile.y) > 0)
				{
					safeTile = false;
				}
			}
			if (safeTile)
			{
				UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
				return;
			}
		}
	}

	// Force Zealots to stay on Tanks and Reavers to shoot if units are near it
	if (unit->getType() == UnitTypes::Protoss_Zealot && target->exists() && (UnitTracker::Instance().getEnUnits()[target].getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || UnitTracker::Instance().getEnUnits()[target].getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && unit->getDistance(targetPosition) < 128)
	{
		UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
		return;
	}

	if (unit->getType() == UnitTypes::Protoss_Reaver && target->exists() && UnitTracker::Instance().getMyUnits()[unit].getGroundRange() > unit->getDistance(target))
	{
		UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
		return;
	}

	// If last command was engage
	if (UnitTracker::Instance().getMyUnits()[unit].getStrategy() == 1)
	{
		// Latch based system for at least 80% disadvantage to disengage
		if (allyLocalStrength < enemyLocalStrength*0.8)
		{
			UnitTracker::Instance().getMyUnits()[unit].setStrategy(0);
			return;
		}
		UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
		return;
	}
	// If last command was disengage/no command
	else
	{
		// Latch based system for at least 120% advantage to engage
		if (allyLocalStrength >= enemyLocalStrength*1.2)
		{
			UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
			return;
		}
		// Otherwise return 3 or 0, whichever was previous
		UnitTracker::Instance().getMyUnits()[unit].setStrategy(0);
		return;
	}
	// Disregard local if no target, no recent local calculation and not within ally region
	UnitTracker::Instance().getMyUnits()[unit].setStrategy(3);
	return;
}

void CommandTrackerClass::getGlobalCalculation(Unit unit, Unit target)
{
	if (StrategyTracker::Instance().isFastExpand())
	{
		globalStrategy = 0;
		return;
	}

	// If Zerg or Toss, wait for a larger army before moving out
	if (Broodwar->enemy()->getRace() != Races::Terran && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver) < 1)
	{
		globalStrategy = 0;
		return;
	}

	if (StrategyTracker::Instance().globalAlly() > StrategyTracker::Instance().globalEnemy())
	{
		globalStrategy = 1;
		return;
	}
	else
	{
		// If Terran, contain
		if (Broodwar->enemy()->getRace() == Races::Terran)
		{
			globalStrategy = 1;
			return;
		}
		globalStrategy = 0;
		return;
	}
	globalStrategy = 1;
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

	WalkPosition start = UnitTracker::Instance().getMyUnits()[unit].getMiniTile();
	Position currentTargetPosition = UnitTracker::Instance().getEnUnits()[unit].getTargetPosition();
	WalkPosition finalPosition = start;
	double highestMobility = 0;

	// Search a 16x16 (4 tiles) mini tile area around the unit for highest mobility	and lowest threat
	for (int x = start.x - 12; x <= start.x + 12 + (unit->getType().tileWidth() * 4); x++)
	{
		for (int y = start.y - 12; y <= start.y + 12 + (unit->getType().tileHeight() * 4); y++)
		{
			if (WalkPosition(x, y).isValid())
			{
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
		if (unit->getOrderTargetPosition() != Position(finalPosition))
		{
			unit->move(Position(finalPosition));
		}
		GridTracker::Instance().updateAllyMovement(unit, finalPosition);
		UnitTracker::Instance().getMyUnits()[unit].setTargetPosition(Position(finalPosition));
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