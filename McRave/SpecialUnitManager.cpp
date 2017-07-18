#include "McRave.h"

void SpecialUnitTrackerClass::update()
{
	Display().startClock();
	updateArbiters();
	updateDetectors();
	updateReavers();
	Display().performanceTest(__FUNCTION__);
	return;
}

void SpecialUnitTrackerClass::updateArbiters()
{
	for (auto &u : myArbiters)
	{
		u.second.setPosition(u.second.unit()->getPosition());
		u.second.setWalkPosition(Util().getWalkPosition(u.second.unit()));
		if (Broodwar->self()->hasResearched(TechTypes::Recall) && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Arbiter) > 1 && u.second.unit()->getEnergy() > 100 && (!recaller || (recaller && !recaller->exists())))
		{
			recaller = u.second.unit();
		}

		int bestCluster = 0;
		double closestD = 0.0;
		Position bestPosition = Grids().getArmyCenter();
		WalkPosition start = u.second.getWalkPosition();

		if (recaller && u.second.unit()->getEnergy() > 100)
		{
			if (u.second.unit()->getDistance(Terrain().getPlayerStartingPosition()) > 320 && Grids().getStasisCluster(u.second.getWalkPosition()) < 4)
			{
				// Move towards areas with no threat close to enemy starting position
				for (int x = start.x - 20; x <= start.x + 20; x++)
				{
					for (int y = start.y - 20; y <= start.y + 20; y++)
					{
						// If the Arbiter finds a tank cluster that is fairly high with little to no air threat, lock in moving there
						if (WalkPosition(x, y).isValid() && Grids().getStasisCluster(WalkPosition(x, y)) >= 4 && Grids().getEAirDistanceGrid(WalkPosition(x, y)) < 10)
						{
							closestD = -1;
							bestPosition = Position(WalkPosition(x, y));
						}

						// Else if the Arbiter finds a tile that is no threat and closer to the enemy starting position
						else if (WalkPosition(x, y).isValid() && (closestD == 0.0 || Terrain().getEnemyStartingPosition().getDistance(Position(WalkPosition(x, y))) < closestD))
						{
							if (Util().isSafe(start, WalkPosition(x, y), UnitTypes::Protoss_Arbiter, false, true, false))
							{
								closestD = Terrain().getEnemyStartingPosition().getDistance(Position(WalkPosition(x, y)));
								bestPosition = Position(WalkPosition(x, y));
							}
						}
					}
				}
			}
			else
			{
				recaller->useTech(TechTypes::Recall, Grids().getArmyCenter());
				Strategy().recallEvent();
				continue;
			}
		}
		else
		{
			// Move towards high cluster counts and closest to ally starting position
			for (int x = start.x - 20; x <= start.x + 20; x++)
			{
				for (int y = start.y - 20; y <= start.y + 20; y++)
				{
					if (WalkPosition(x, y).isValid() && Grids().getEMPGrid(x, y) == 0 && Grids().getArbiterGrid(x, y) == 0 && (closestD == 0.0 || Grids().getACluster(x, y) > bestCluster || (Grids().getACluster(x, y) == bestCluster && Terrain().getPlayerStartingPosition().getDistance(Position(WalkPosition(x, y))) < closestD)))
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
		}
		// Move and update grids	
		u.second.setDestination(bestPosition);
		u.first->move(bestPosition);
		Grids().updateArbiterMovement(u.first);

		// If there's a stasis target, cast stasis on it
		Unit target = Units().getAllyUnits()[u.first].getTarget();
		if (target && target->exists() && u.first->getEnergy() >= 100)
		{
			u.first->useTech(TechTypes::Stasis_Field, target);
		}
	}
	return;
}

void SpecialUnitTrackerClass::updateDetectors()
{
	for (auto &u : myDetectors)
	{
		u.second.setPosition(u.second.unit()->getPosition());
		u.second.setWalkPosition(Util().getWalkPosition(u.second.unit()));

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
		WalkPosition start = u.second.getWalkPosition();
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
		if (newDestination.isValid())
		{
			u.second.setDestination(newDestination);
			u.first->move(newDestination);
			Grids().updateObserverMovement(u.first);
		}
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
		u.second.setPosition(u.second.unit()->getPosition());
		u.second.setWalkPosition(Util().getWalkPosition(u.second.unit()));
	}
	return;
}

void SpecialUnitTrackerClass::storeUnit(Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Arbiter)
	{
		myArbiters[unit].setUnit(unit);
	}
	else if (unit->getType() == UnitTypes::Protoss_Observer)
	{
		myDetectors[unit].setUnit(unit);
	}
	else if (unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		myTemplars[unit].setUnit(unit);
	}
	else if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		myReavers[unit].setUnit(unit);
	}
	return;
}

void SpecialUnitTrackerClass::removeUnit(Unit unit)
{
	if (myArbiters.find(unit) != myArbiters.end())
	{
		myArbiters.erase(unit);
	}
	else if (myDetectors.find(unit) != myDetectors.end())
	{
		myDetectors.erase(unit);
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

