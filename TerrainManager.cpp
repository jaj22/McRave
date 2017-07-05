#include "McRave.h"

void TerrainTrackerClass::update()
{
	clock_t myClock;
	double duration = 0.0;
	myClock = clock();

	// Create island regions and natural expansion	
	for (auto &area : theMap.Areas())
	{
		for (auto &base : area.Bases())
		{
			if (area.AccessibleNeighbours().size() == 0)
			{
				islandRegions.emplace(area.Id());
			}
			else
			{
				allBaseLocations.emplace(base.Location());
			}
		}
	}

	// Start location
	playerStartingTilePosition = Broodwar->self()->getStartLocation();
	playerStartingPosition = Position(playerStartingTilePosition);

	// If we see a building, check for closest starting location
	if (enemyBasePositions.size() <= 0)
	{
		for (auto &unit : Units().getEnUnits())
		{
			if (unit.second.getType().isBuilding() && Terrain().getEnemyBasePositions().size() == 0 && unit.second.getPosition().getDistance(Terrain().getPlayerStartingPosition()) > 1600)
			{
				double distance = 0.0;
				TilePosition closest;
				for (auto &base : theMap.StartingLocations())
				{
					if (unit.second.getPosition().getDistance(Position(base)) < distance || distance == 0.0)
					{
						distance = unit.second.getPosition().getDistance(Position(base));
						closest = base;
					}
				}
				if (closest.isValid())
				{
					enemyBasePositions.emplace(Position(closest));
					enemyStartingTilePosition = closest;
					enemyStartingPosition = Position(closest);
					path = theMap.GetPath(playerStartingPosition, enemyStartingPosition);
				}
			}
		}
	}

	for (auto &base : Bases().getMyBases())
	{
		if (base.second.getTilePosition().isValid() && theMap.GetArea(base.second.getTilePosition()))
		{
			allyTerritory.emplace(theMap.GetArea(base.second.getTilePosition())->Id());
		}
	}

	for (auto &base : enemyBasePositions)
	{
		if (base.isValid() && Broodwar->isVisible(TilePosition(base)) && Broodwar->getUnitsInRadius(base, 128, Filter::IsEnemy).size() == 0)
		{
			enemyBasePositions.erase(base);
			break;
		}
	}

	// Establish FFE position
	if (Broodwar->getFrameCount() > 100)
	{
		int x = 0;
		int y = 0;
		const Area* closestA;
		double closestD = 0.0;
		TilePosition closestP;
		for (auto &area : theMap.Areas())
		{
			for (auto &base : area.Bases())
			{
				if (base.Geysers().size() == 0 || area.AccessibleNeighbours().size() == 0)
				{
					continue;
				}
				if (Grids().getDistanceHome(WalkPosition(base.Location())) > 50 && (Grids().getDistanceHome(WalkPosition(base.Location())) < closestD || closestD == 0))
				{
					closestD = Grids().getDistanceHome(WalkPosition(base.Location()));
					closestA = base.GetArea();
				}
			}
		}
		if (closestA)
		{
			for (auto &choke : closestA->ChokePoints())
			{
				x += Position(choke->Center()).x;
				y += Position(choke->Center()).y;

			}
			x += closestP.x;
			y += closestP.y;

			x = x / closestA->ChokePoints().size();
			y = y / closestA->ChokePoints().size();
			FFEPosition = TilePosition(Position(x, y));
		}
	}

	// Draw path for debugging
	for (auto &choke : path)
	{
		Broodwar->drawCircleMap(Position(choke->Center()), 32, Colors::Red);
	}

	duration = 1000.0 * (clock() - myClock) / (double)CLOCKS_PER_SEC;
	//Broodwar->drawTextScreen(200, 80, "Terrain Manager: %d ms", duration);
}

bool TerrainTrackerClass::isInAllyTerritory(Unit unit)
{
	if (unit && unit->exists() && unit->getTilePosition().isValid() && theMap.GetArea(unit->getTilePosition()))
	{
		if (allyTerritory.find(theMap.GetArea(unit->getTilePosition())->Id()) != allyTerritory.end())
		{
			return true;
		}
	}
	return false;
}

void TerrainTrackerClass::removeTerritory(Unit base)
{
	if (base)
	{
		if (enemyBasePositions.find(base->getPosition()) != enemyBasePositions.end())
		{
			enemyBasePositions.erase(base->getPosition());

			if (theMap.GetArea(base->getTilePosition()))
			{
				if (allyTerritory.find(theMap.GetArea(base->getTilePosition())->Id()) != allyTerritory.end())
				{
					allyTerritory.erase(theMap.GetArea(base->getTilePosition())->Id());
				}
			}
		}
	}
}