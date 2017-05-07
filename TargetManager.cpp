#include "TargetManager.h"

void UnitTrackerClass::unitGetTarget(Unit unit)
{
	double highest = 0.0, thisUnit = 0.0;

	Unit target = nullptr;

	for (auto &u : enemyUnits)
	{
		if (!u.first)
		{
			continue;
		}

		// If unit is dead or unattackble based on flying
		if (u.second.getDeadFrame() > 0 || (u.second.getUnitType().isFlyer() && (unit->getType() == UnitTypes::Protoss_Zealot || unit->getType() == UnitTypes::Protoss_Reaver)))
		{
			continue;
		}

		if (u.first->exists() && u.first->isStasised())
		{
			continue;
		}

		double distance = 1.0 + double(unit->getDistance(u.second.getPosition()));

		if (u.first->exists())
		{
			thisUnit = u.second.getMaxStrength() / distance;
		}
		else
		{
			thisUnit = 0.1*u.second.getMaxStrength() / distance;
		}


		// Reduce building threat
		if (u.second.getUnitType().isBuilding())
		{
			thisUnit = 0.1*thisUnit;
		}

		// If this is the strongest enemy around, target it
		if (thisUnit > highest || highest == 0)
		{
			target = u.first;
			highest = thisUnit;
		}
	}

	// If the target is not nullptr, store
	if (target)
	{
		allyUnits[unit].setTarget(target);
		allyUnits[unit].setTargetPosition(enemyUnits[target].getPosition());
	}
	return;
}

void UnitTrackerClass::unitGetClusterTarget(Unit unit)
{
	// Cluster variables, range of spells is 10
	int highest = 0, range = 10;
	TilePosition clusterTile;

	// Reaver range is 8
	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		range = 8;
	}

	// Iterate through every tile in range to see what clusters are in range
	for (int x = unit->getTilePosition().x - range; x <= unit->getTargetPosition().x + range; x++)
	{
		for (int y = unit->getTilePosition().y - range; y <= unit->getTilePosition().y + range; y++)
		{
			if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight())
			{
				// Reavers want ground clusters
				if (unit->getType() == UnitTypes::Protoss_Reaver)
				{
					if (GridTracker::Instance().getEGroundCluster(x,y) > highest)
					{
						highest = GridTracker::Instance().getEGroundCluster(x, y);
						clusterTile = TilePosition(x, y);
					}
				}
				// Arbiters want Siege Tank clusters
				else if (unit->getType() == UnitTypes::Protoss_Arbiter)
				{
					if (GridTracker::Instance().getTankCluster(x, y) > highest)
					{
						highest = GridTracker::Instance().getTankCluster(x, y);
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
	// If there is no cluster, return a getTarget unit
	if (highest < 2)
	{
		if (unit->getType() == UnitTypes::Protoss_Reaver)
		{
			return UnitTrackerClass::unitGetTarget(unit);
		}
		else
		{
			return;
		}
	}
	// Return ground cluster for Reavers
	else if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		allyUnits[unit].setTarget(Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsFlyer, 128));
	}
	// Return tank cluster for Arbiters
	else if (unit->getType() == UnitTypes::Protoss_Arbiter)
	{
		allyUnits[unit].setTarget(Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding && (Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode), 128));
	}
	// Return unit cluster for High Templars
	else if (unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		allyUnits[unit].setTarget(Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding, 128));
	}
	return;
}