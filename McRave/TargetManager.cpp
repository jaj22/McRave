#include "McRave.h"

Unit TargetTrackerClass::getTarget(Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Reaver || unit->getType() == UnitTypes::Protoss_High_Templar || unit->getType() == UnitTypes::Protoss_Arbiter)
	{
		return clusterTarget(unit);
	}
	else if (unit->getType() == UnitTypes::Protoss_Observer || unit->getType() == UnitTypes::Protoss_Shuttle)
	{
		return nullptr;
	}
	else if (unit->getType() == UnitTypes::Terran_Medic)
	{
		return allyTarget(unit);
	}
	else
	{
		return singleTarget(unit);
	}
	return nullptr;
}

Unit TargetTrackerClass::singleTarget(Unit unit)
{
	double highest = 0.0, thisUnit = 0.0;
	Unit target = nullptr;

	for (auto &u : Units().getEnUnits())
	{
		if (!u.first)
		{
			continue;
		}

		// If unit is dead or unattackble based on flying
		if (u.second.getDeadFrame() > 0 || (u.second.getType().isFlyer() && Util().getTrueAirDamage(unit->getType(), Broodwar->self()) == 0) || (!u.second.getType().isFlyer() && Util().getTrueGroundDamage(unit->getType(), Broodwar->self()) == 0))
		{
			continue;
		}

		// If the enemy is stasised, ignore it
		if (u.first->exists() && u.first->isStasised())
		{
			continue;
		}

		// If the unit is invis and undetected, ignore it
		if (u.first->exists() && (u.first->isCloaked() || u.first->isBurrowed()) && !u.first->isDetected())
		{
			continue;
		}

		double distance = double(unit->getDistance(u.second.getPosition()));

		if (u.first->exists())
		{
			thisUnit = u.second.getPriority() / distance;
		}
		else
		{
			thisUnit = 0.1*u.second.getPriority() / distance;
		}

		if (u.second.getType().isBuilding())
		{
			thisUnit = thisUnit * 0.25;
		}

		//thisUnit = u.second.getPriority() / (1.0 + (distance * Grids().getEGroundDistanceGrid(u.second.getWalkPosition())));

		// If this is the strongest enemy around, target it
		if (thisUnit > highest || highest == 0.0)
		{
			target = u.first;
			highest = thisUnit;
		}
	}
	if (target)
	{
		Units().getMyUnits()[unit].setTargetPosition(Units().getEnUnits()[target].getPosition());
	}
	return target;
}

Unit TargetTrackerClass::allyTarget(Unit unit)
{
	double highest = 0.0;
	Unit target = nullptr;

	// Search for an ally target that needs healing for medics
	for (auto &a : Units().getMyUnits())
	{
		UnitInfo ally = a.second;
		if (!ally.unit() || ally.getDeadFrame() != 0 || !ally.getType().isOrganic())
		{
			continue;
		}
		
		if (ally.unit()->isBeingHealed() && Units().getMyUnits()[unit].getTarget() != ally.unit())
		{
			continue;
		}

		double distance = 1.0 + double(unit->getDistance(ally.getPosition()));

		if (ally.unit()->exists() && ally.unit()->getHitPoints() < ally.getType().maxHitPoints() && (distance < highest || highest == 0.0))
		{
			highest = distance;
			target = ally.unit();
		}
	}

	// If we found an ally target, store the position
	if (target)
	{
		Units().getMyUnits()[unit].setTargetPosition(Units().getMyUnits()[target].getPosition());
	}
	return target;
}


Unit TargetTrackerClass::clusterTarget(Unit unit)
{
	// Cluster variables, range of spells is 10
	int highest = 0, range = 10;
	TilePosition clusterTile;
	Units().getMyUnits()[unit].setTargetPosition(Positions::None);
	Units().getMyUnits()[unit].setTarget(nullptr);

	// Reaver range is 8
	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		range = 8;
	}

	// Iterate through every tile in range to see what clusters are in range
	for (int x = unit->getTilePosition().x - range; x <= unit->getTilePosition().x + range; x++)
	{
		for (int y = unit->getTilePosition().y - range; y <= unit->getTilePosition().y + range; y++)
		{
			if (TilePosition(x, y).isValid())
			{
				// Reavers want ground clusters
				if (unit->getType() == UnitTypes::Protoss_Reaver)
				{
					if (Grids().getEGroundCluster(x, y) > highest)
					{
						highest = Grids().getEGroundCluster(x, y);
						clusterTile = TilePosition(x, y);
					}
				}
				// Arbiters want Siege Tank clusters
				else if (unit->getType() == UnitTypes::Protoss_Arbiter)
				{
					if (Grids().getStasisCluster(x, y) > highest)
					{
						highest = Grids().getStasisCluster(x, y);
						clusterTile = TilePosition(x, y);
					}
				}
				// High Templars can have air or ground clusters
				else if (unit->getType() == UnitTypes::Protoss_High_Templar && Grids().getACluster(WalkPosition(TilePosition(x,y))) == 0)
				{
					if (Grids().getEGroundCluster(x, y) > highest)
					{
						highest = Grids().getEGroundCluster(x, y);
						clusterTile = TilePosition(x, y);
					}
					if (Grids().getEAirCluster(x, y) > highest)
					{
						highest = Grids().getEAirCluster(x, y);
						clusterTile = TilePosition(x, y);
					}
				}
			}
		}
	}
	// If there is no cluster, return a single target for Reavers
	if (highest < 2)
	{
		if (unit->getType() == UnitTypes::Protoss_Reaver)
		{
			return singleTarget(unit);
		}
		else
		{
			return nullptr;
		}
	}
	// Return ground cluster for Reavers
	else if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		Unit target = Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsFlyer, 256);
		if (target)
		{
			Units().getMyUnits()[unit].setTargetPosition(Units().getEnUnits()[target].getPosition());
		}
		return target;
	}
	// Return tank cluster for Arbiters
	else if (unit->getType() == UnitTypes::Protoss_Arbiter)
	{
		Unit target = Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding && (Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode), 128);
		if (target)
		{
			Units().getMyUnits()[unit].setTargetPosition(Units().getEnUnits()[target].getPosition());
		}
		return target;
	}
	// Return unit cluster for High Templars
	else if (unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		Unit target = Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding && !Filter::IsUnderStorm, 128);
		if (target)
		{
			Units().getMyUnits()[unit].setTargetPosition(Units().getEnUnits()[target].getPosition());
		}
		return target;
	}
	return nullptr;
}