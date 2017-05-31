#include "SpecialUnitManager.h"
#include "TerrainManager.h"
#include "GridManager.h"
#include "UnitManager.h"
#include "BuildOrder.h"

void SpecialUnitTrackerClass::update()
{
	updateArbiters();
	updateObservers();
	updateTemplars();
	return;
}

void SpecialUnitTrackerClass::updateArbiters()
{
	for (auto & u : myArbiters)
	{
		int initial_x = TilePosition(u.second.getPosition()).x;
		int initial_y = TilePosition(u.second.getPosition()).y;
		Position newDestination = u.second.getDestination();
		double closestD = u.second.getDestination().getDistance(TerrainTracker::Instance().getEnemyStartingPosition());
		for (int x = initial_x - 12; x <= initial_x + 12; x++)
		{
			for (int y = initial_y - 12; y <= initial_y + 12; y++)
			{
				if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && GridTracker::Instance().getArbiterGrid(x, y) == 0 && GridTracker::Instance().getEAirGrid(x, y) == 0 && GridTracker::Instance().getACluster(x, y) > 0 && Position(TilePosition(x, y)).getDistance(TerrainTracker::Instance().getPlayerStartingPosition()) < closestD)
				{
					newDestination = Position(TilePosition(x, y));
				}
			}
		}

		// Move and update grids
		u.second.setDestination(newDestination);
		u.first->move(newDestination);
		GridTracker::Instance().updateArbiterGrids();

		Unit target = UnitTracker::Instance().getMyUnits()[u.first].getTarget();
		if (target)
		{
			u.first->useTech(TechTypes::Stasis_Field, target);
		}
	}
	return;
}

void SpecialUnitTrackerClass::updateObservers()
{
	for (auto & u : myObservers)
	{
		// First check if any expansions need detection on them
		if (BuildOrderTracker::Instance().getBuildingDesired()[UnitTypes::Protoss_Nexus] > Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus))
		{
			for (auto base : TerrainTracker::Instance().getNextExpansion())
			{
				// If an expansion is unbuildable and we've scouted it already, move there to detect burrowed units
				if (!Broodwar->canBuildHere(base, UnitTypes::Protoss_Nexus, nullptr, true))
				{
					u.second.setDestination(Position(base));
					u.first->move(Position(base));
					GridTracker::Instance().updateObserverMovement(u.first);
					return;
				}
			}
		}

		// Then find an optimal location to move to within a 25x25 tile area
		// Optimal defined as: no ally observer around, no enemy air, at least 1 ally around and as close to the enemy as possible
		// TODO: Add enemy detection to optimal
		WalkPosition start = u.second.getMiniTile();
		Position newDestination = u.second.getPosition();
		double closestD = u.second.getPosition().getDistance(TerrainTracker::Instance().getEnemyStartingPosition());
		for (int x = start.x - 20; x <= start.x + 20; x++)
		{
			for (int y = start.y - 20; y <= start.y + 20; y++)
			{
				if (WalkPosition(x, y).isValid() && GridTracker::Instance().getACluster(x,y) > 0 && GridTracker::Instance().getObserverGrid(x, y) == 0 && GridTracker::Instance().getEAirGrid(x, y) == 0.0 && Position(WalkPosition(x, y)).getDistance(TerrainTracker::Instance().getEnemyStartingPosition()) < closestD)
				{
					newDestination = Position(WalkPosition(x, y));
					closestD = Position(WalkPosition(x, y)).getDistance(TerrainTracker::Instance().getEnemyStartingPosition());
				}
			}
		}
		u.second.setDestination(newDestination);
		u.first->move(newDestination);
		GridTracker::Instance().updateObserverMovement(u.first);
		Broodwar->drawLineMap(u.second.getPosition(), u.second.getDestination(), Broodwar->self()->getColor());
		continue;
	}
	return;
}

void SpecialUnitTrackerClass::updateTemplars()
{
	for (auto & u : myTemplars)
	{
		Unit target = UnitTracker::Instance().getMyUnits()[u.first].getTarget();
		int stratL = UnitTracker::Instance().getMyUnits()[u.first].getStrategy();
		if (stratL == 1 || stratL == 0)
		{
			if (target)
			{
				if (u.first->getEnergy() > 75)
				{
					u.first->useTech(TechTypes::Psionic_Storm, target);
					return;
				}
				else if (u.first->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_High_Templar) && (u.first->getEnergy() < 70 || u.first->isUnderAttack()))
				{
					u.first->useTech(TechTypes::Archon_Warp, u.first->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_High_Templar));
					return;
				}
			}
		}
	}
	return;
}

void SpecialUnitTrackerClass::storeUnit(Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Arbiter)
	{
		myArbiters[unit].setPosition(unit->getPosition());
		myArbiters[unit].setDestination(unit->getPosition());
		myArbiters[unit].setMiniTile(UnitTracker::Instance().getMiniTile(unit));
	}
	else if (unit->getType() == UnitTypes::Protoss_Observer)
	{
		myObservers[unit].setPosition(unit->getPosition());
		myObservers[unit].setDestination(unit->getPosition());
		myObservers[unit].setMiniTile(UnitTracker::Instance().getMiniTile(unit));
	}
	else if (unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		myTemplars[unit].setPosition(unit->getPosition());
		myTemplars[unit].setDestination(unit->getPosition());
		myTemplars[unit].setMiniTile(UnitTracker::Instance().getMiniTile(unit));
	}
	return;
}

void SpecialUnitTrackerClass::removeUnit(Unit unit)
{
	if (myArbiters.find(unit) != myArbiters.end())
	{
		myArbiters.erase(unit);
	}
	else if (myObservers.find(unit) != myObservers.end())
	{
		myObservers.erase(unit);
	}
	else if (myTemplars.find(unit) != myTemplars.end())
	{
		myTemplars.erase(unit);
	}
}