#include "McRave.h"

void TransportTrackerClass::update()
{
	for (auto & shuttle : myShuttles)
	{
		updateCargo(shuttle.second);
		updateDecision(shuttle.second);
		updateMovement(shuttle.second);
	}
	return;
}

void TransportTrackerClass::updateCargo(TransportInfo& shuttle)
{
	// Check if the shuttle has space remaining
	if (shuttle.getCargoSize() < 4)
	{
		// See if any Reavers need a shuttle
		for (auto & reaver : SpecialUnitTracker::Instance().getMyReavers())
		{
			if (!UnitTracker::Instance().getMyUnits()[reaver.first].hasTransport() && shuttle.getCargoSize() + 2 < 4)
			{
				UnitTracker::Instance().getMyUnits()[reaver.first].setTransport(true);
				shuttle.assignCargo(reaver.first);
			}
		}
		// See if any High Templars need a shuttle
		for (auto & templar : SpecialUnitTracker::Instance().getMyTemplars())
		{
			if (!UnitTracker::Instance().getMyUnits()[templar.first].hasTransport() && shuttle.getCargoSize() + 1 < 4)
			{
				UnitTracker::Instance().getMyUnits()[templar.first].setTransport(true);
				shuttle.assignCargo(templar.first);
			}
		}
	}
	return;
}

void TransportTrackerClass::updateDecision(TransportInfo& shuttle)
{
	// Check if we should harass or fight with main army
	if (StrategyTracker::Instance().globalAlly() > StrategyTracker::Instance().globalEnemy())
	{
		shuttle.setDrop(TerrainTracker::Instance().getEnemyStartingPosition());
		shuttle.setHarassing(true);
	}
	else
	{
		shuttle.setDrop(GridTracker::Instance().getArmyCenter());
		shuttle.setHarassing(false);
	}

	// Check if we should be loading/unloading any cargo
	for (auto & cargo : shuttle.getAssignedCargo())
	{
		// Reset load state
		shuttle.setLoadState(0);

		// If the cargo is not loaded
		if (!cargo->isLoaded())
		{
			// If it's requesting a pickup
			if ((UnitTracker::Instance().getMyUnits()[cargo].getStrategy() != 1 || (cargo->getType() == Protoss_Reaver && cargo->getGroundWeaponCooldown() > Broodwar->getLatencyFrames()) || (cargo->getType() == Protoss_High_Templar && cargo->getEnergy() < 75)))
			{
				shuttle.unit()->load(cargo);
				shuttle.setLoadState(1);
				Broodwar->drawLineMap(shuttle.getPosition(), cargo->getPosition(), Broodwar->self()->getColor());
				continue;
			}
		}
		// Else if the cargo is loaded
		else if (cargo->isLoaded())
		{
			// If we are harassing, check if we are close to drop point
			if (shuttle.isHarassing() && shuttle.getPosition().getDistance(shuttle.getDrop()) < 160 && GridTracker::Instance().getEGroundGrid(shuttle.getMiniTile()) <= 0.0 && GridTracker::Instance().getEAirGrid(shuttle.getMiniTile().x, shuttle.getMiniTile().y) <= 0.0)
			{
				shuttle.unit()->unload(cargo);
				shuttle.setLoadState(2);
			}
			// Else check if we are in a position to help the main army
			else if (!shuttle.isHarassing() && UnitTracker::Instance().getMyUnits()[cargo].getStrategy() == 1 && UnitTracker::Instance().getMyUnits()[cargo].getPosition().getDistance(UnitTracker::Instance().getMyUnits()[cargo].getTargetPosition()) < 320 && (cargo->getGroundWeaponCooldown() <= Broodwar->getLatencyFrames() || cargo->getEnergy() >= 75))
			{
				shuttle.unit()->unload(cargo);
				shuttle.setLoadState(2);
			}
		}
	}
	return;
}

void TransportTrackerClass::updateMovement(TransportInfo& shuttle)
{
	// If performing a loading action, don't update movement
	if (shuttle.getLoadState() != 0)
	{
		return;
	}

	// Initial setup
	Position bestPosition = shuttle.getDrop();
	WalkPosition start = shuttle.getMiniTile();
	double bestCluster = 0;
	double closestD = 0.0;
	for (int itr = 1; itr <= 50; itr++)
	{
		for (int x = start.x - itr; x <= start.x + itr; x++)
		{
			for (int y = start.y - itr; y <= start.y + itr; y++)
			{
				if (!WalkPosition(x, y).isValid())
				{
					continue;
				}

				if (WalkPosition(x, y) == start)
				{
					continue;
				}

				// If position is too close to maintain movement speed or has an air threat, don't move there
				if (GridTracker::Instance().getEAirGrid(x, y) > 0)
				{
					continue;
				}

				// If trying to unload, must find a walkable tile
				if (shuttle.getLoadState() == 1 && GridTracker::Instance().getMobilityGrid(x, y) == 0)
				{
					continue;
				}

				// If shuttle is harassing, move along low mobility low threat tiles
				if (shuttle.isHarassing())
				{
					if ((1 + GridTracker::Instance().getEGroundGrid(x, y)) * (1 + shuttle.getDrop().getDistance(Position(WalkPosition(x, y)))) * (1 + GridTracker::Instance().getMobilityGrid(x, y)) < closestD || closestD == 0.0)
					{
						closestD = (1 + GridTracker::Instance().getEGroundGrid(x, y)) * (1 + shuttle.getDrop().getDistance(Position(WalkPosition(x, y)))) * (1 + GridTracker::Instance().getMobilityGrid(x, y));
						bestPosition = Position(WalkPosition(x, y));
					}
				}
				// Else, move to high ally cluster areas
				else if (WalkPosition(x, y).getDistance(start) > 5 && (GridTracker::Instance().getACluster(x, y) > bestCluster || (GridTracker::Instance().getACluster(x, y) == bestCluster && bestCluster != 0 && shuttle.getDrop().getDistance(Position(WalkPosition(x, y))) < closestD)))
				{
					closestD = shuttle.getDrop().getDistance(Position(WalkPosition(x, y)));
					bestCluster = GridTracker::Instance().getACluster(x, y);
					bestPosition = Position(WalkPosition(x, y));
				}
			}
		}
	}

	shuttle.unit()->move(bestPosition);
	shuttle.setDestination(bestPosition);
	Broodwar->drawLineMap(shuttle.getPosition(), shuttle.getDestination(), Broodwar->self()->getColor());
	return;
}

void TransportTrackerClass::removeUnit(Unit unit)
{
	// Delete if it's a shuttled unit
	for (auto shuttle : myShuttles)
	{
		if (shuttle.second.getAssignedCargo().find(unit) != shuttle.second.getAssignedCargo().end())
		{
			shuttle.second.removeCargo(unit);
			return;
		}
	}
	if (myShuttles.find(unit) != myShuttles.end())
	{
		myShuttles.erase(unit);
	}
	return;
}

void TransportTrackerClass::storeUnit(Unit unit)
{
	myShuttles[unit].setUnit(unit);
	myShuttles[unit].setPosition(unit->getPosition());
	myShuttles[unit].setMiniTile(UnitUtil::Instance().getMiniTile(unit));
}