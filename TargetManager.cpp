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
	else
	{
		return singleTarget(unit);
	}
}

Unit TargetTrackerClass::singleTarget(Unit unit)
{
	double highest = 0.0, thisUnit = 0.0;
	Unit target = nullptr;

	for (auto &u : UnitTracker::Instance().getEnUnits())
	{
		if (!u.first)
		{
			continue;
		}

		// If unit is dead or unattackble based on flying
		if (u.second.getDeadFrame() > 0 || (u.second.getType().isFlyer() && (unit->getType() == UnitTypes::Protoss_Zealot || unit->getType() == UnitTypes::Protoss_Reaver)))
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

		double distance = 1.0 + double(unit->getDistance(u.second.getPosition()));

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

		// If this is the strongest enemy around, target it
		if (thisUnit > highest || highest == 0.0)
		{
			target = u.first;
			highest = thisUnit;
		}
	}	
	return target;
}


Unit TargetTrackerClass::clusterTarget(Unit unit)
{
	// Cluster variables, range of spells is 10
	int highest = 0, range = 10;
	TilePosition clusterTile;
	UnitTracker::Instance().getMyUnits()[unit].setTargetPosition(Positions::None);
	UnitTracker::Instance().getMyUnits()[unit].setTarget(nullptr);

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
					if (GridTracker::Instance().getEGroundCluster(x, y) > highest)
					{
						highest = GridTracker::Instance().getEGroundCluster(x, y);
						clusterTile = TilePosition(x, y);
					}
				}
				// Arbiters want Siege Tank clusters
				else if (unit->getType() == UnitTypes::Protoss_Arbiter)
				{
					if (GridTracker::Instance().getStasisCluster(x, y) > highest)
					{
						highest = GridTracker::Instance().getStasisCluster(x, y);
						clusterTile = TilePosition(x, y);
					}
				}
				// High Templars can have air or ground clusters
				else if (unit->getType() == UnitTypes::Protoss_High_Templar)
				{
					if (GridTracker::Instance().getEGroundCluster(x, y) > highest)
					{
						highest = GridTracker::Instance().getEGroundCluster(x, y);
						clusterTile = TilePosition(x, y);
					}
					if (GridTracker::Instance().getEAirCluster(x, y) > highest)
					{
						highest = GridTracker::Instance().getEAirCluster(x, y);
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
		return Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsFlyer, 256);
	}
	// Return tank cluster for Arbiters
	else if (unit->getType() == UnitTypes::Protoss_Arbiter)
	{
		return Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding && (Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode), 128);
	}
	// Return unit cluster for High Templars
	else if (unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		return Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding && !Filter::IsUnderStorm, 128);
	}
	return nullptr;
}