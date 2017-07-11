#include "McRave.h"

Unit TargetTrackerClass::getTarget(UnitInfo& unit)
{
	if (unit.getType() == UnitTypes::Protoss_Observer || unit.getType() == UnitTypes::Protoss_Shuttle)
	{
		return nullptr;
	}
	else if (unit.getType() == UnitTypes::Terran_Medic)
	{
		return allyTarget(unit);
	}
	else
	{
		return enemyTarget(unit);
	}
	return nullptr;
}

Unit TargetTrackerClass::enemyTarget(UnitInfo& unit)
{
	double highest = 0.0, thisUnit = 0.0;
	Unit target = nullptr;
	Position targetPosition;

	for (auto &e : Units().getEnUnits())
	{
		UnitInfo enemy = e.second;
		if (!enemy.unit())
		{
			continue;
		}

		// If unit is dead or unattackable
		if (enemy.getDeadFrame() > 0 || (enemy.getType().isFlyer() && unit.getAirRange() == 0) || (!enemy.getType().isFlyer() && unit.getGroundRange() == 0))
		{
			continue;
		}


		if (unit.getType().isFlyer())
		{
			thisUnit = enemy.getPriority() / (1.0 + (unit.getPosition().getDistance(enemy.getPosition()) * Grids().getEAirDistanceGrid(enemy.getWalkPosition())));
		}
		else
		{
			// If the unit has higher range and is faster
			if (enemy.getType() == UnitTypes::Terran_Vulture && unit.getGroundRange() < enemy.getGroundRange())
			{
				continue;
			}

			// If the enemy is stasised, ignore it
			if (enemy.unit()->exists() && enemy.unit()->isStasised())
			{
				continue;
			}

			// If the unit is invis and undetected, ignore it
			if (enemy.unit()->exists() && (enemy.unit()->isCloaked() || enemy.unit()->isBurrowed()) && !enemy.unit()->isDetected())
			{
				continue;
			}			


			thisUnit = enemy.getPriority() / (1.0 + (unit.getPosition().getDistance(enemy.getPosition())));

			// Cluster targeting
			/*if (unit.getType() == UnitTypes::Protoss_Reaver || unit.getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode)
			{
				thisUnit = (enemy.getPriority() * Grids().getEGroundCluster(enemy.getTilePosition())) / (1.0 + (unit.getPosition().getDistance(enemy.getPosition()) * Grids().getEGroundDistanceGrid(enemy.getWalkPosition())));
			}
			else if (unit.getType() == UnitTypes::Protoss_Arbiter)
			{
				thisUnit = (enemy.getPriority() * Grids().getStasisCluster(enemy.getTilePosition())) / (1.0 + (unit.getPosition().getDistance(enemy.getPosition()) * Grids().getEGroundDistanceGrid(enemy.getWalkPosition())));
			}
			else if (unit.getType() == UnitTypes::Protoss_High_Templar)
			{
				if (Grids().getACluster(enemy.getWalkPosition()) == 0)
				{
					thisUnit = (enemy.getPriority() * Grids().getEGroundCluster(enemy.getTilePosition()) * Grids().getEAirCluster(enemy.getTilePosition())) / (1.0 + (unit.getPosition().getDistance(enemy.getPosition()) * Grids().getEGroundDistanceGrid(enemy.getWalkPosition())));
				}
			}
			else
			{
				thisUnit = enemy.getPriority() / (1.0 + (unit.getPosition().getDistance(enemy.getPosition())));
			}*/
		}

		// If this is the strongest enemy around, target it
		if (thisUnit > highest || highest == 0.0)
		{
			target = enemy.unit();
			highest = thisUnit;
			targetPosition = enemy.getPosition();
		}
	}
	if (target)
	{
		unit.setTargetPosition(targetPosition);
	}
	return target;
}

Unit TargetTrackerClass::allyTarget(UnitInfo& unit)
{
	double highest = 0.0;
	Unit target = nullptr;
	Position targetPosition;

	// Search for an ally target that needs healing for medics
	for (auto &a : Units().getMyUnits())
	{
		UnitInfo ally = a.second;
		if (!ally.unit() || ally.getDeadFrame() != 0 || !ally.getType().isOrganic())
		{
			continue;
		}

		if (ally.unit()->isBeingHealed() && unit.getTarget() != ally.unit())
		{
			continue;
		}

		double distance = unit.getPosition().getDistance(ally.getPosition());

		if (ally.unit()->exists() && ally.unit()->getHitPoints() < ally.getType().maxHitPoints() && (distance < highest || highest == 0.0))
		{
			highest = distance;
			target = ally.unit();
			targetPosition = ally.getPosition();
		}
	}

	// If we found an ally target, store the position
	if (target)
	{
		unit.setTargetPosition(targetPosition);
	}
	return target;
}