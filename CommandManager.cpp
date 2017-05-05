#include "CommandManager.h"

void UnitTrackerClass::commandUpdate()
{
	for (auto u : allyUnits)
	{
		if (u.first->getType().isWorker() || u.first->getType().isBuilding())
		{
			continue;
		}
		// Ignore the unit if it no longer exists, is locked down, maelstrommed, stassised, not powered or not completed
		if (!u.first || !u.first->exists() || u.first->isLockedDown() || u.first->isMaelstrommed() || u.first->isStasised() || !u.first->isPowered() || !u.first->isCompleted())
		{
			continue;
		}
		unitGetCommand(u.first);
	}
}

void UnitTrackerClass::unitGetCommand(Unit unit)
{
	allyUnits[unit].setTargetPosition(Positions::None);
	allyUnits[unit].setLocal(0);
	unitGetTarget(unit);

	double closestD = 0.0;
	Position closestP;
	Unit target = allyUnits[unit].getTarget();

	if (!target || target == nullptr)
	{
		return;
	}

	if (unit->getType() == UnitTypes::Protoss_Reaver && unit->getScarabCount() < 5)
	{
		unit->train(UnitTypes::Protoss_Scarab);
	}

	unitGetLocalStrategy(unit, target);
	int stratG = unitGetGlobalStrategy();
	int stratL = allyUnits[unit].getStrategy();

	// If target and unit are both valid and we're not ignoring local calculations
	if (stratL != 3)
	{
		// Attack
		if (stratL == 1 && target->exists())
		{
			unitMicroTarget(unit, target);
			return;
		}
		// Retreat
		if (stratL == 0)
		{
			// Force engage Zealots on ramp
			if (allyTerritory.size() <= 1 && unit->getDistance(defendHere.at(0)) < 64 && unit->getType() == UnitTypes::Protoss_Zealot && unit->getUnitsInRadius(64, Filter::IsEnemy).size() > 0)
			{
				unitMicroTarget(unit, target);
				return;
			}

			// Create concave when containing units
			if (enemyGroundStrengthGrid[unit->getTilePosition().x][unit->getTilePosition().y] == 0.0 && stratG == 1)
			{
				Position fleePosition = unitFlee(unit, target);
				if (fleePosition != Positions::None)
				{
					unit->move(Position(fleePosition.x + rand() % 3 + (-1), fleePosition.y + rand() % 3 + (-1)));
				}
				return;
			}

			// For each defensive position, find closest one
			for (auto position : defendHere)
			{
				if (unit->getDistance(position) < 320 || allyTerritory.find(getRegion(unit->getTilePosition())) != allyTerritory.end())
				{
					Position fleePosition = unitFlee(unit, target);
					if (fleePosition != Positions::None)
					{
						unit->move(Position(fleePosition.x + rand() % 3 + (-1), fleePosition.y + rand() % 3 + (-1)));
					}
					return;
				}
				if (unit->getDistance(position) <= closestD || closestD == 0.0)
				{
					closestD = unit->getDistance(position);
					closestP = position;
				}
			}

			// If last command was too far away from this position, move there
			if (unit->getLastCommand().getTargetPosition().getDistance(defendHere.at(0)) > 5)
			{
				unit->move(Position(defendHere.at(0).x + rand() % 3 + (-1), defendHere.at(0).y + rand() % 3 + (-1)));
			}
			return;
		}
	}

	if (stratG == 0)
	{
		if (enemyBasePositions.size() > 0 && allyTerritory.size() > 0)
		{
			// Pick random enemy bases to attack (cap at ~3-4 units?)
			closestD = 1000.0;
			closestP = defendHere.at(0);

			// If we forced to expand, move to next choke to prevent blocking 
			if (forceExpand == 1)
			{
				closestP = Position(path.at(1)->Center());
				if (allyTerritory.find(getNearestChokepoint(TilePosition(path.at(1)->Center()))->getRegions().second) != allyTerritory.end() || allyTerritory.find(getNearestChokepoint(TilePosition(path.at(1)->Center()))->getRegions().first) != allyTerritory.end())
				{
					closestP = Position(path.at(2)->Center());
				}
			}
			else
			{
				// Check if we are close enough to any defensive position
				for (auto position : defendHere)
				{
					if (unit->getDistance(position) < 128)
					{
						closestP = position;
						break;
					}
					else if (unit->getDistance(position) <= closestD)
					{
						closestD = unit->getDistance(position);
						closestP = position;
					}
				}
			}
			if (unit->getLastCommand().getTargetPosition().getDistance(closestP) > 5 || unit->getLastCommandFrame() + 100 < Broodwar->getFrameCount())
			{
				unit->move(Position(closestP.x + rand() % 3 + (-1), closestP.y + rand() % 3 + (-1)));
				return;
			}
		}
		else
		{
			// Else just defend at nearest chokepoint (starting of game without scout information)
			unit->move((getNearestChokepoint(unit->getPosition()))->getCenter());
			return;
		}
	}

	// Check if we should attack
	if (stratG == 1 && enemyBasePositions.size() > 0)
	{
		if (target && target->exists() && unit->getDistance(target) < 512)
		{
			UnitTrackerClass::unitMicroTarget(unit, target);
			return;
		}
		unit->attack(enemyBasePositions.front());
		return;
	}
}

void UnitTrackerClass::unitGetLocalStrategy(Unit unit, Unit target)
{
	double thisUnit = 0.0;
	double enemyLocalStrength = 0.0, allyLocalStrength = 0.0;
	Position targetPosition = enemyUnits[target].getPosition();
	int radius = min(512, 384 + supply * 4);

	if (unit->getDistance(targetPosition) > 512)
	{
		allyUnits[unit].setStrategy(3);
		return;
	}

	// Check every enemy unit being in range of the target
	for (auto u : UnitTracker::Instance().getEnUnits())
	{
		// Reset unit strength
		thisUnit = 0.0;

		// Ignore workers, keep buildings (reinforcements and static defenses)
		if (u.second.getUnitType().isWorker() || (u.first && u.first->exists() && u.first->isStasised()))
		{
			continue;
		}

		// If a unit is within range of the target, add to local strength
		if (u.second.getPosition().getDistance(targetPosition) < radius)
		{
			if (aLarge > 0 || aMedium > 0 || aSmall > 0)
			{
				// If unit is cloaked or burrowed and not detected, drastically increase strength
				if ((u.first->isCloaked() || u.first->isBurrowed()) && !u.first->isDetected())
				{
					thisUnit = 20 * u.second.getStrength();
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
	for (auto u : UnitTracker::Instance().getMyUnits())
	{
		// Reset unit strength
		thisUnit = 0.0;

		// Ignore workers and buildings
		if (u.second.getUnitType().isWorker() || u.second.getUnitType().isBuilding())
		{
			continue;
		}

		// If a unit is within the range of the ally unit, add to local strength
		if (u.second.getPosition().getDistance(unit->getPosition()) < radius)
		{
			if (eLarge > 0 || eMedium > 0 || eSmall > 0)
			{
				// If shuttle, add units inside
				if (u.second.getUnitType() == UnitTypes::Protoss_Shuttle && u.first->getLoadedUnits().size() > 0)
				{
					// Assume reaver for damage type calculations
					for (Unit uL : u.first->getLoadedUnits())
					{
						thisUnit = allyUnits[uL].getStrength();
					}
				}
				else
				{
					// Damage type calculations
					if (u.second.getUnitType().groundWeapon().damageType() == DamageTypes::Explosive)
					{
						thisUnit = u.second.getStrength() * ((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / (eLarge + eMedium + eSmall);
					}
					else if (u.second.getUnitType().groundWeapon().damageType() == DamageTypes::Concussive)
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
	allyUnits[unit].setLocal(allyLocalStrength - enemyLocalStrength);


	// If we are in ally territory and have a target, force to fight	
	if (target && target->exists())
	{
		if (allyTerritory.find(getRegion(target->getPosition())) != allyTerritory.end())
		{
			allyUnits[unit].setStrategy(1);
			return;
		}
	}

	// Force Zealots to stay on Tanks
	if (unit->getType() == UnitTypes::Protoss_Zealot && target->exists() && (enemyUnits[target].getUnitType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || enemyUnits[target].getUnitType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && unit->getDistance(targetPosition) < 128)
	{
		allyUnits[unit].setStrategy(1);
		return;
	}

	// If unit is in range of a target and not currently threatened, attack instead of running
	if (unit->getDistance(targetPosition) <= allyUnits[unit].getRange() && (enemyGroundStrengthGrid[unit->getTilePosition().x][unit->getTilePosition().y] == 0 || unit->getType() == UnitTypes::Protoss_Reaver))
	{
		allyUnits[unit].setStrategy(1);
		return;
	}

	// If last command was engage
	if (allyUnits[unit].getStrategy() == 1)
	{
		// Latch based system for at least 80% disadvantage to disengage
		if (allyLocalStrength < enemyLocalStrength*0.8)
		{
			allyUnits[unit].setStrategy(0);
			return;
		}
		allyUnits[unit].setStrategy(1);
		allyUnits[unit].setTargetPosition(targetPosition);
		return;
	}
	// If last command was disengage/no command
	else
	{
		// Latch based system for at least 120% advantage to engage
		if (allyLocalStrength >= enemyLocalStrength*1.2)
		{
			allyUnits[unit].setTargetPosition(targetPosition);
			allyUnits[unit].setStrategy(1);
			return;
		}
		// Otherwise return 3 or 0, whichever was previous
		allyUnits[unit].setStrategy(0);
		return;
	}
	// Disregard local if no target, no recent local calculation and not within ally region
	allyUnits[unit].setStrategy(3);
	return;
}

int UnitTrackerClass::unitGetGlobalStrategy()
{
	if (forceExpand == 1)
	{
		return 0;
	}
	if (StrategyTracker::Instance().globalAlly() > StrategyTracker::Instance().globalEnemy())	
	{
		// If Zerg, wait for a larger army before moving out
		if (Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) == 0)
		{
			return 0;
		}
		return 1;
	}
	else
	{
		// If not Zerg, contain
		if (Broodwar->enemy()->getRace() != Races::Zerg)
		{
			return 1;
		}
		return 0;
	}
	return 1;
}

void UnitTrackerClass::unitMicroTarget(Unit unit, Unit target)
{
	// Variables
	bool kite = false;
	int range = allyUnits[unit].getRange();
	int offset = 0;

	// Stop offset
	if (unit->getType() == UnitTypes::Protoss_Dragoon)
	{
		offset = 9;
	}
	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		offset = 1;
	}

	if (unit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit->getTarget() == target)
	{
		return;
	}

	// If kiting unnecessary, disable
	if (target->getType().isBuilding() || unit->getType() == UnitTypes::Protoss_Corsair)
	{
		kite = false;
	}

	// If kiting is a good idea, enable
	else if (target->getType() == UnitTypes::Terran_Vulture_Spider_Mine || (range > 32 && unit->isUnderAttack()) || (target->getType().groundWeapon().maxRange() <= range && (unit->getDistance(target) < range - target->getType().groundWeapon().maxRange() && target->getType().groundWeapon().maxRange() > 0 && unit->getType().groundWeapon().maxRange() > 32 || unit->getHitPoints() < 40)))
	{
		kite = true;
	}

	// If kite is true and weapon on cooldown, move
	if (kite && Broodwar->getFrameCount() - allyUnits[unit].getLastCommandFrame() > offset - Broodwar->getRemainingLatencyFrames() && unit->getGroundWeaponCooldown() > 0)
	{
		Position correctedFleePosition = unitFlee(unit, target);
		// Want Corsairs to move closer always if possible
		if (unit->getType() == UnitTypes::Protoss_Corsair)
		{
			unit->move(target->getPosition());
			allyUnits[unit].setTargetPosition(target->getPosition());
		}
		else if (correctedFleePosition != BWAPI::Positions::None && (unit->getLastCommand().getType() != UnitCommandTypes::Move || unit->getLastCommand().getTargetPosition().getDistance(correctedFleePosition) > 5))
		{
			unit->move(Position(correctedFleePosition.x + rand() % 3 + (-1), correctedFleePosition.y + rand() % 3 + (-1)));
			allyUnits[unit].setTargetPosition(correctedFleePosition);
		}
	}
	// Else, regardless of if kite is true or not, attack if weapon is off cooldown
	else if (unit->getGroundWeaponCooldown() <= 0)
	{
		unit->attack(target);
	}
	return;
}

void UnitTrackerClass::unitExploreArea(Unit unit)
{
	// Given a region, explore a random portion of it based on random metrics like:
	// Distance to enemy
	// Distance to home
	// Last explored
	// Unit deaths
	// Untouched resources
}

