#include "McRave.h"

void CommandTrackerClass::update()
{
	Display().startClock();
	updateAlliedUnits();
	Display().performanceTest(__FUNCTION__);
	return;
}

void CommandTrackerClass::updateAlliedUnits()
{
	for (auto &u : Units().getMyUnits())
	{
		UnitInfo unit = u.second;

		// Special units have their own commands
		if (unit.getType() == UnitTypes::Protoss_Observer || unit.getType() == UnitTypes::Protoss_Arbiter || unit.getType() == UnitTypes::Protoss_Shuttle)
		{
			continue;
		}

		// Ignore the unit if it no longer exists, is locked down, maelstrommed, stassised, or not completed
		if (!unit.unit() || (unit.unit() && !unit.unit()->exists()))
		{
			continue;
		}
		if (unit.unit()->exists() && (unit.unit()->isLockedDown() || unit.unit()->isMaelstrommed() || unit.unit()->isStasised() || !unit.unit()->isCompleted()))
		{
			continue;
		}

		// If the unit is ready to perform an action after an attack (certain units have minimum frames after an attack before they can receive a new command)
		if (Broodwar->getFrameCount() - unit.getLastAttackFrame() > unit.getMinStopFrame() - Broodwar->getLatencyFrames())
		{
			// If globally behind
			if (Units().getGlobalStrategy() == 0 || Units().getGlobalStrategy() == 2)
			{
				// Check if we have a target
				if (unit.getTarget())
				{
					// If locally ahead, fight
					if (unit.getStrategy() == 1 && unit.getTarget()->exists())
					{
						attackTarget(unit);
						continue;
					}
					// Else flee
					else if (unit.getStrategy() == 0)
					{
						fleeTarget(unit);
						continue;
					}
				}
				// Else defend
				defend(unit);
				continue;
			}

			// If globally ahead
			else if (Units().getGlobalStrategy() == 1)
			{
				// Check if we have a target
				if (unit.getTarget())
				{
					// If locally behind, contain
					if (unit.getStrategy() == 0 || unit.getStrategy() == 2)
					{
						fleeTarget(unit);
						continue;
					}
					// Else attack
					else if (unit.getStrategy() == 1 && unit.getTarget()->exists())
					{
						attackTarget(unit);
						continue;
					}
					// Else attack move best location
					else
					{
						attackMove(unit);
						continue;
					}
				}
			}
			// Else attack move
			attackMove(unit);
			continue;
		}
	}
	return;
}

void CommandTrackerClass::attackMove(UnitInfo& unit)
{
	// If target doesn't exist, move towards it
	if (unit.getTarget() && unit.getTargetPosition().isValid())
	{
		if (unit.unit()->getOrderTargetPosition() != unit.getTargetPosition() || unit.unit()->isStuck())
		{
			if (unit.getGroundDamage() > 0 || unit.getAirDamage() > 0)
			{
				unit.unit()->attack(unit.getTargetPosition());
			}
			else
			{
				unit.unit()->move(unit.getTargetPosition());
			}
		}
		return;
	}

	// Else if no target, attack closest enemy base if there is any
	else if (Terrain().getEnemyBasePositions().size() > 0)
	{
		Position here = Terrain().getClosestEnemyBase(unit.getPosition());
		if (here.isValid())
		{
			if (unit.unit()->getOrderTargetPosition() != here || unit.unit()->isStuck())
			{
				if (unit.getGroundDamage() > 0 || unit.getAirDamage() > 0)
				{
					unit.unit()->attack(here);
				}
				else
				{
					unit.unit()->move(here);
				}
			}
		}
	}

	// Else attack a random base location
	else
	{
		int random = rand() % (Terrain().getAllBaseLocations().size() - 1);
		int i = 0;
		if (unit.unit()->isIdle())
		{
			for (auto &base : Terrain().getAllBaseLocations())
			{
				if (i == random)
				{
					unit.unit()->attack(Position(base));
					return;
				}
				else
				{
					i++;
				}
			}
		}
	}
	return;
}

void CommandTrackerClass::attackTarget(UnitInfo& unit)
{
	// TEMP -- Set to false initially
	kite = false;

	// Specific High Templar behavior
	if (unit.getType() == UnitTypes::Protoss_High_Templar)
	{
		if (unit.getTarget() && unit.getTarget()->exists() && unit.unit()->getEnergy() >= 75)
		{
			unit.unit()->useTech(TechTypes::Psionic_Storm, unit.getTarget());
			return;
		}
	}

	// Specific Marine and Firebat behavior	
	if ((unit.getType() == UnitTypes::Terran_Marine || unit.getType() == UnitTypes::Terran_Firebat) && !unit.unit()->isStimmed() && unit.getTargetPosition().isValid() && unit.unit()->getDistance(unit.getTargetPosition()) <= unit.getGroundRange())
	{
		unit.unit()->useTech(TechTypes::Stim_Packs);
	}

	// Specific Medic behavior
	if (unit.getType() == UnitTypes::Terran_Medic)
	{
		if (unit.unit()->getLastCommand().getType() != UnitCommandTypes::Use_Tech_Unit || unit.unit()->getLastCommand().getTarget() != unit.getTarget())
		{
			unit.unit()->useTech(TechTypes::Healing, unit.getTarget());
		}
		return;
	}

	// Specific Tank behavior
	if (unit.getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode)
	{
		if (unit.unit()->getDistance(unit.getTargetPosition()) <= 400 && unit.unit()->getDistance(unit.getTargetPosition()) > 128)
		{
			unit.unit()->useTech(TechTypes::Tank_Siege_Mode);
		}
	}
	if (unit.getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode)
	{
		if (unit.unit()->getDistance(unit.getTargetPosition()) > 400 || unit.unit()->getDistance(unit.getTargetPosition()) < 128)
		{
			unit.unit()->useTech(TechTypes::Tank_Siege_Mode);
		}
	}

	// If we need to use static defenses
	if (Grids().getBatteryGrid(unit.getTilePosition()) > 0 && ((unit.unit()->getLastCommand().getType() == UnitCommandTypes::Right_Click_Unit && unit.unit()->getShields() < 40) || unit.unit()->getShields() < 10))
	{
		if (unit.unit()->getLastCommand().getType() != UnitCommandTypes::Right_Click_Unit)
		{
			for (auto battery : Buildings().getMyBatteries())
			{
				if (battery.second.getEnergy() >= 10 && unit.unit()->getDistance(battery.second.getPosition()) < 320)
				{
					unit.unit()->rightClick(battery.second.unit());
					continue;
				}
			}
		}
		return;
	}

	if (Grids().getBunkerGrid(unit.getTilePosition()) > 0)
	{
		Unit bunker = unit.unit()->getClosestUnit(Filter::GetType == UnitTypes::Terran_Bunker && Filter::SpaceRemaining > 0);
		if (bunker)
		{
			unit.unit()->rightClick(bunker);
		}
	}

	// If kiting unnecessary, disable
	if (unit.getTarget()->getType().isBuilding() || unit.getType() == UnitTypes::Protoss_Corsair)
	{
		kite = false;
	}

	// Reavers should always kite away from their target if it has lower range
	else if (unit.getType() == UnitTypes::Protoss_Reaver && Units().getEnUnits()[unit.getTarget()].getGroundRange() < unit.getGroundRange())
	{
		kite = true;
	}	

	// If kiting is a good idea, enable
	else if ((unit.getGroundRange() > 32 && unit.unit()->isUnderAttack()) || (Units().getEnUnits()[unit.getTarget()].getGroundRange() <= unit.getGroundRange() && (unit.unit()->getDistance(unit.getTargetPosition()) <= unit.getGroundRange() - Units().getEnUnits()[unit.getTarget()].getGroundRange() && Units().getEnUnits()[unit.getTarget()].getGroundRange() > 0 && unit.getGroundRange() > 32 || unit.unit()->getHitPoints() < 40)))
	{
		kite = true;
	}

	// If kite is true and weapon on cooldown, move
	if (kite && unit.unit()->getGroundWeaponCooldown() > 0)
	{
		fleeTarget(unit);
		return;
	}
	else if (unit.unit()->getGroundWeaponCooldown() <= 0)
	{
		// If unit receieved an attack command on the target already, don't give another order
		if (unit.unit()->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit.unit()->getLastCommand().getTarget() == unit.getTarget())
		{
			return;
		}
		if (unit.unit()->getOrderTarget() != unit.getTarget() || unit.unit()->isStuck())
		{
			unit.unit()->attack(unit.getTarget());
		}
		unit.setTargetPosition(Units().getEnUnits()[unit.getTarget()].getPosition());
	}
	return;
}

void CommandTrackerClass::exploreArea(UnitInfo& unit)
{
	// Given a region, explore a random portion of it based on random metrics like:
	// Distance to enemy
	// Distance to home
	// Last explored
	// Unit deaths
	// Untouched resources
}

void CommandTrackerClass::fleeTarget(UnitInfo& unit)
{
	WalkPosition start = unit.getWalkPosition();
	WalkPosition finalPosition = start;
	double highestMobility = 0.0;
	int offset = int(unit.getSpeed()) / 8;

	// Specific High Templar flee
	/*if (unit.getType() == UnitTypes::Protoss_High_Templar && (unit.unit()->getEnergy() < 75 || unit.unit()->isUnderAttack()))
	{
		for (auto templar : SpecialUnits().getMyTemplars())
		{
			if (templar.second.unit()->getEnergy() < 75 || templar.second.unit()->isUnderAttack())
			{
				unit.unit()->useTech(TechTypes::Archon_Warp, templar.second.unit());
				return;
			}
		}
	}*/

	// Search a circle around the target based on the speed of the unit in one second of game time
	for (int x = start.x - 20; x <= start.x + 20 + (unit.getType().tileWidth() * 4); x++)
	{
		for (int y = start.y - 20; y <= start.y + 20 + (unit.getType().tileHeight() * 4); y++)
		{
			if (!WalkPosition(x, y).isValid())
			{
				continue;
			}
			if (unit.getPosition().getDistance(Position(WalkPosition(x, y))) < 80)
			{
				if (unit.getType() == UnitTypes::Protoss_Dragoon && Grids().getResourceGrid(x / 4, y / 4) > 0)
				{
					continue;
				}

				double mobility = double(Grids().getMobilityGrid(x, y));
				double threat = Grids().getEGroundGrid(x, y);
				double distance = Grids().getEGroundDistanceGrid(x, y);
				double distanceHome = double(pow(Grids().getDistanceHome(x, y), 0.1));

				if ((mobility / (1.0 + (distance * threat))) / distanceHome > highestMobility && Util().isSafe(start, WalkPosition(x, y), unit.getType(), false, false, true))
				{
					highestMobility = (mobility / (1.0 + (distance * threat))) / distanceHome;
					finalPosition = WalkPosition(x, y);
				}
			}
		}
	}

	// If a valid position was found that isn't the starting position
	if (finalPosition.isValid() && finalPosition != start)
	{
		Grids().updateAllyMovement(unit.unit(), finalPosition);
		unit.setTargetPosition(Position(finalPosition));
		if (unit.unit()->getLastCommand().getTargetPosition() != Position(finalPosition) || unit.unit()->isStuck())
		{
			unit.unit()->move(Position(finalPosition));
		}
	}
	return;
}

void CommandTrackerClass::defend(UnitInfo& unit)
{
	// Early on, defend mineral line
	if (Terrain().getEnemyBasePositions().size() == 0 || !Strategy().isHoldRamp())
	{
		for (auto &base : Bases().getMyBases())
		{
			if (unit.unit()->getLastCommand().getTargetPosition() != (Position(base.second.getResourcesPosition()) + Position(base.second.getPosition())) / 2 || unit.unit()->getLastCommand().getType() != UnitCommandTypes::Move || unit.unit()->isStuck())
			{
				unit.unit()->move((Position(base.second.getResourcesPosition()) + Position(base.second.getPosition())) / 2);
			}
			return;
		}
	}

	// Defend chokepoint with concave
	int min = 128;
	int max = 320;
	double closestD = 0.0;
	WalkPosition start = unit.getWalkPosition();
	WalkPosition bestPosition;
	if (unit.getGroundRange() <= 32)
	{
		min = 64;
		max = 128;
	}

	// Find closest chokepoint
	WalkPosition choke;
	for (auto &base : Bases().getMyBases())
	{
		if (base.second.getTilePosition().isValid() && theMap.GetArea(base.second.getTilePosition()))
		{
			for (auto &chokepoint : theMap.GetArea(base.second.getTilePosition())->ChokePoints())
			{
				if (chokepoint->BlockingNeutral())
				{
					continue;
				}
				if (Grids().getDistanceHome(chokepoint->Center()) > closestD || closestD == 0.0)
				{
					closestD = Grids().getDistanceHome(chokepoint->Center());
					choke = chokepoint->Center();
				}
			}
		}
	}

	// Find suitable position to hold at chokepoint
	closestD = unit.getPosition().getDistance(Position(choke));
	for (int x = choke.x - 25; x <= choke.x + 25; x++)
	{
		for (int y = choke.y - 25; y <= choke.y + 25; y++)
		{
			if (WalkPosition(x, y).isValid() && Grids().getMobilityGrid(x, y) > 0 && theMap.GetArea(WalkPosition(x, y)) && Terrain().getAllyTerritory().find(theMap.GetArea(WalkPosition(x, y))->Id()) != Terrain().getAllyTerritory().end() && Position(WalkPosition(x, y)).getDistance(Position(choke)) > min && Position(WalkPosition(x, y)).getDistance(Position(choke)) < max && Position(WalkPosition(x, y)).getDistance(Position(choke)) < closestD)
			{
				if (Util().isSafe(start, WalkPosition(x, y), unit.getType(), false, false, true))
				{
					bestPosition = WalkPosition(x, y);
					closestD = Position(WalkPosition(x, y)).getDistance(Position(choke));
				}
			}
		}
	}
	if (bestPosition.isValid() && (unit.unit()->getOrderTargetPosition() != Position(bestPosition) || unit.unit()->getLastCommand().getType() != UnitCommandTypes::Move || unit.unit()->isStuck()))
	{
		unit.setTargetPosition(Position(bestPosition));
		unit.unit()->move(Position(bestPosition));
		Grids().updateAllyMovement(unit.unit(), bestPosition);
	}
	return;
}