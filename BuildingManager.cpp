#include "McRave.h"

void BuildingTrackerClass::update()
{
	clock_t myClock;
	double duration = 0.0;
	myClock = clock();

	queueBuildings();
	constructBuildings();

	duration = 1000.0 * (clock() - myClock) / (double)CLOCKS_PER_SEC;
	//Broodwar->drawTextScreen(200, 0, "Building Manager: %d ms", duration);

	int closestD = 0;
	TilePosition closestP;
	if (Strategy().isFastExpand())
	{
		for (auto &area : theMap.Areas())
		{
			for (auto &base : area.Bases())
			{
				if (Grids().getDistanceHome(WalkPosition(base.Location())) > 50 && (Grids().getDistanceHome(WalkPosition(base.Location())) < closestD || closestD == 0))
				{
					closestD = Grids().getDistanceHome(WalkPosition(base.Location()));
					closestP = base.Location();
				}
			}
		}	
		Broodwar->drawCircleMap(Position(closestP), 32, Colors::Red);
	}
}

void BuildingTrackerClass::queueBuildings()
{
	// For each building desired
	for (auto &b : BuildOrder().getBuildingDesired())
	{
		// If our visible count is lower than our desired count
		if (b.second > Broodwar->self()->visibleUnitCount(b.first) && b.second - Broodwar->self()->visibleUnitCount(b.first) > buildingsQueued[b.first])
		{
			TilePosition here = Buildings().getBuildLocation(b.first);
			Unit builder = Workers().getClosestWorker(Position(here));

			// If the Tile Position and Builder are valid
			if (here.isValid() && builder)
			{
				// Queue at this building type a pair of building placement and builder		
				Workers().getMyWorkers()[builder].setBuildingType(b.first);
				Workers().getMyWorkers()[builder].setBuildPosition(here);
			}
		}
	}
}

void BuildingTrackerClass::constructBuildings()
{
	queuedMineral = 0;
	queuedGas = 0;
	for (auto &building : buildingsQueued)
	{
		building.second = 0;
	}
	for (auto &worker : Workers().getMyWorkers())
	{
		if (worker.second.getBuildingType().isValid() && worker.second.getBuildPosition().isValid())
		{
			buildingsQueued[worker.second.getBuildingType()] += 1;
			queuedMineral = queuedMineral + worker.second.getBuildingType().mineralPrice();
			queuedGas = queuedGas + worker.second.getBuildingType().gasPrice();
		}
	}
	for (auto &building : myBuildings)
	{
		if (building.second.getUnitType().getRace() == Races::Terran && !building.first->isCompleted() && !building.first->getBuildUnit())
		{
			Unit builder = Workers().getClosestWorker(building.second.getPosition());
			if (builder)
			{
				builder->rightClick(building.first);
			}
			continue;
		}
	}
}

void BuildingTrackerClass::storeBuilding(Unit building)
{
	if (myBuildings.find(building) != myBuildings.end())
	{
		myBuildings[building].setUnit(building);
		myBuildings[building].setUnitType(building->getType());
		myBuildings[building].setPosition(building->getPosition());
		myBuildings[building].setWalkPosition(Util().getWalkPosition(building));
		myBuildings[building].setTilePosition(building->getTilePosition());
	}
	myBuildings[building].setIdleStatus(building->getRemainingTrainTime() == 0);
	myBuildings[building].setEnergy(building->getEnergy());
}

void BuildingTrackerClass::removeBuilding(Unit building)
{
	myBuildings.erase(building);
}

TilePosition BuildingTrackerClass::getBuildLocationNear(UnitType building, TilePosition buildTilePosition, bool ignoreCond = false)
{
	int x = buildTilePosition.x;
	int y = buildTilePosition.y;
	int length = 1;
	int j = 0;
	bool first = true;
	int dx = 0;
	int dy = 1;

	// Searches in a spiral around the specified tile position
	while (length < 200)
	{
		//
		if (Strategy().isFastExpand() && Grids().getDistanceHome(WalkPosition(TilePosition(x, y))) < Grids().getDistanceHome(WalkPosition(buildTilePosition)))
		{

		}

		// If we can build here, return this tile position		
		else if (TilePosition(x, y).isValid() && Util().canBuildHere(building, TilePosition(x, y), ignoreCond))
		{
			return TilePosition(x, y);
		}

		// Otherwise spiral out and find a new tile
		x = x + dx;
		y = y + dy;
		j++;
		if (j == length)
		{
			j = 0;
			if (!first)
				length++;
			first = !first;
			if (dx == 0)
			{
				dx = dy;
				dy = 0;
			}
			else
			{
				dy = -dx;
				dx = 0;
			}
		}
	}
	Broodwar << "Out of tiles" << endl;
	return TilePositions::None;
}

TilePosition BuildingTrackerClass::getBuildLocation(UnitType building)
{
	// If we are expanding, it must be on an expansion area
	int closestD = 0;
	TilePosition closestP;
	if (building.isResourceDepot())
	{
		for (auto &area : theMap.Areas())
		{
			for (auto &base : area.Bases())
			{
				if (Grids().getReserveGrid(base.Location()) == 0 && (Grids().getDistanceHome(WalkPosition(base.Location())) < closestD || closestD == 0))
				{
					closestD = Grids().getDistanceHome(WalkPosition(base.Location()));
					closestP = base.Location();
				}
			}
		}
		return closestP;
	}

	// If we are fast expanding
	if (Strategy().isFastExpand())
	{
		for (auto &area : theMap.Areas())
		{
			for (auto &base : area.Bases())
			{
				if (base.Geysers().size() == 0)
				{
					continue;
				}
				if (Grids().getDistanceHome(WalkPosition(base.Location())) > 50 && (Grids().getDistanceHome(WalkPosition(base.Location())) < closestD || closestD == 0))
				{
					closestD = Grids().getDistanceHome(WalkPosition(base.Location()));
					closestP = base.Location();
				}
			}
		}
		return getBuildLocationNear(building, closestP);
	}

	// For each base, check if there's a Pylon or Cannon needed
	for (auto &base : Bases().getMyBases())
	{
		if (building == UnitTypes::Protoss_Pylon && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) >= 2)
		{
			if (Grids().getPylonGrid(base.second.getTilePosition()) == 0)
			{
				return getBuildLocationNear(building, base.second.getTilePosition());
			}
		}
		if (building == UnitTypes::Protoss_Photon_Cannon)
		{
			if (Grids().getDefenseGrid(base.second.getTilePosition()) < Grids().getDistanceHome(base.second.getWalkPosition()) / 100)
			{
				return getBuildLocationNear(building, base.second.getResourcesPosition());
			}
		}
	}

	// For each base, check if you can build near it
	for (auto &base : Bases().getMyOrderedBases())
	{
		TilePosition here = getBuildLocationNear(building, base.second);
		if (here.isValid())
		{
			return here;
		}
	}
	return TilePositions::None;
}