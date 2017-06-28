#include "McRave.h"

void TerrainTrackerClass::update()
{
	clock_t myClock;
	double duration = 0.0;
	myClock = clock();

	// Create island regions and natural expansion
	TilePosition natural;
	double closest = 0.0;
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

	// Start location and path
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
		allyTerritory.emplace(theMap.GetArea(base.second.getTilePosition())->Id());
	}

	for (auto &base : enemyBasePositions)
	{		
		if (Broodwar->isVisible(TilePosition(base)) && Broodwar->getUnitsInRadius(base, 128, Filter::IsEnemy).size() == 0)
		{
			enemyBasePositions.erase(find(enemyBasePositions.begin(), enemyBasePositions.end(), base));
			break;
		}
	}

	duration = 1000.0 * (clock() - myClock) / (double)CLOCKS_PER_SEC;
	//Broodwar->drawTextScreen(200, 80, "Terrain Manager: %d ms", duration);
}

void TerrainTrackerClass::removeTerritory(Unit base)
{
	if (enemyBasePositions.find(base->getPosition()) != enemyBasePositions.end())
	{
		enemyBasePositions.erase(base->getPosition());
	}
	if (theMap.GetArea(base->getTilePosition()))
	{
		if (allyTerritory.find(theMap.GetArea(base->getTilePosition())->Id()) != allyTerritory.end())
		{
			allyTerritory.erase(theMap.GetArea(base->getTilePosition())->Id());
		}
	}
}