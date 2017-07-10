#include "McRave.h"

void SpecialUnitTrackerClass::update()
{
	Display().startClock();
	updateArbiters();
	updateObservers();
	updateReavers();
	Display().performanceTest(__FUNCTION__);
	return;
}

void SpecialUnitTrackerClass::updateArbiters()
{
	for (auto &u : myArbiters)
	{
		// Move towards high cluster counts and closest to ally starting position
		int bestCluster = 0;
		double closestD = 0.0;
		Position bestPosition = Grids().getArmyCenter();
		WalkPosition start = u.second.getMiniTile();
		for (int x = start.x - 20; x <= start.x + 20; x++)
		{
			for (int y = start.y - 20; y <= start.y + 20; y++)
			{
				if (WalkPosition(x, y).isValid() && Grids().getArbiterGrid(x, y) == 0 && (closestD == 0.0 || Grids().getACluster(x, y) > bestCluster || (Grids().getACluster(x, y) == bestCluster && Terrain().getPlayerStartingPosition().getDistance(Position(WalkPosition(x, y))) < closestD)))
				{
					if (Util().isSafe(start, WalkPosition(x, y), UnitTypes::Protoss_Arbiter, false, true, false))
					{
						closestD = Terrain().getPlayerStartingPosition().getDistance(Position(WalkPosition(x, y)));
						bestCluster = Grids().getACluster(x, y);
						bestPosition = Position(WalkPosition(x, y));
					}
				}
			}
		}
		// Move and update grids	
		u.second.setDestination(bestPosition);
		u.first->move(bestPosition);
		Grids().updateArbiterMovement(u.first);

		// If there's a stasis target, cast stasis on it
		UnitInfo target = Units().getEnUnits()[u.first];
		if (target.unit() && target.unit()->exists() && u.first->getEnergy() >= 100)
		{
			u.first->useTech(TechTypes::Stasis_Field, target.unit());
		}
	}
	return;
}

void SpecialUnitTrackerClass::updateObservers()
{
	for (auto &u : myObservers)
	{
		// First check if any expansions need detection on them
		if (BuildOrder().getBuildingDesired()[UnitTypes::Protoss_Nexus] > Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus))
		{
			bool baseScout = false;
			for (auto &base : Terrain().getAllBaseLocations())
			{
				// If an expansion is unbuildable and we've scouted it already, move there to detect burrowed units
				if (!Broodwar->canBuildHere(base, UnitTypes::Protoss_Nexus, nullptr) && Grids().getBaseGrid(base) == 0)
				{
					u.second.setDestination(Position(base));
					u.first->move(Position(base));
					Grids().updateObserverMovement(u.first);
					baseScout = true;
				}
			}
			if (baseScout)
			{
				continue;
			}
		}

		// Move towards lowest enemy air threat, no enemy detection and closest to enemy starting position	
		double closestD = 0.0;
		Position newDestination = Grids().getArmyCenter();
		WalkPosition start = u.second.getMiniTile();
		for (int x = start.x - 20; x <= start.x + 20; x++)
		{
			for (int y = start.y - 20; y <= start.y + 20; y++)
			{
				if (WalkPosition(x, y).isValid() && Grids().getEDetectorGrid(x, y) == 0 && Position(WalkPosition(x, y)).getDistance(Position(start)) > 64 && Grids().getACluster(x, y) > 0 && Grids().getObserverGrid(x, y) == 0 && Grids().getEAirGrid(x, y) == 0.0 && (Position(WalkPosition(x, y)).getDistance(Terrain().getEnemyStartingPosition()) < closestD || closestD == 0))
				{
					newDestination = Position(WalkPosition(x, y));
					closestD = Position(WalkPosition(x, y)).getDistance(Terrain().getEnemyStartingPosition());
				}
			}
		}
		u.second.setDestination(newDestination);
		u.first->move(newDestination);
		Grids().updateObserverMovement(u.first);
		//Broodwar->drawLineMap(u.second.getPosition(), u.second.getDestination(), Broodwar->self()->getColor());
		//Broodwar->drawBoxMap(u.second.getDestination() - Position(4, 4), u.second.getDestination() + Position(4, 4), Broodwar->self()->getColor(), true);
		continue;
	}
	return;
}

void SpecialUnitTrackerClass::updateReavers()
{
	for (auto &u : myReavers)
	{
		// If we need Scarabs
		if (u.first->getScarabCount() < 5)
		{
			u.first->train(UnitTypes::Protoss_Scarab);
		}
	}
}

void SpecialUnitTrackerClass::storeUnit(Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Arbiter)
	{
		myArbiters[unit].setPosition(unit->getPosition());
		myArbiters[unit].setWalkPosition(Util().getWalkPosition(unit));
	}
	else if (unit->getType() == UnitTypes::Protoss_Observer)
	{
		myObservers[unit].setPosition(unit->getPosition());
		myObservers[unit].setWalkPosition(Util().getWalkPosition(unit));
	}
	else if (unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		myTemplars[unit].setPosition(unit->getPosition());
		myTemplars[unit].setWalkPosition(Util().getWalkPosition(unit));
	}
	else if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		myReavers[unit].setPosition(unit->getPosition());
		myReavers[unit].setWalkPosition(Util().getWalkPosition(unit));
	}
	else if (unit->getType() == UnitTypes::Terran_Medic)
	{
		myMedics[unit].setPosition(unit->getPosition());
		myMedics[unit].setWalkPosition(Util().getWalkPosition(unit));
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
	else if (myReavers.find(unit) != myReavers.end())
	{
		myReavers.erase(unit);
	}
	return;
}

