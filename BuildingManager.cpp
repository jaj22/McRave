#include "BuildingManager.h"
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace std;
using namespace BWTA;

// Variables for BuildingManager.cpp
// Changes: Ignore building conditions for static defenses (make bool in function)
TilePosition buildTilePosition;

TilePosition nexusManager()
{
	for (auto base : nextExpansion)
	{
		if (Broodwar->getUnitsInRadius(Position(base), 128, Filter::IsResourceDepot).size() <= 0)
		{
			return base;
		}
	}
	return TilePositions::None;
}

TilePosition cannonManager(TilePosition staticP, UnitType building)
{
	return getBuildLocationNear(building, staticP, true);
}

TilePosition buildingManager(UnitType building)
{
	if (building == UnitTypes::Protoss_Nexus)
	{
		return nexusManager();
	}

	// For each expansion, check if you can build near it, starting at the main
	for (TilePosition tile : activeExpansion)
	{
		if (building == UnitTypes::Protoss_Assimilator)
		{
			for (auto gas : geysers)
			{
				if (Broodwar->canBuildHere(gas->getTilePosition(), UnitTypes::Protoss_Assimilator))
				{
					return gas->getTilePosition();
				}
			}
		}
		else
		{
			buildTilePosition = getBuildLocationNear(building, tile, false);
		}
		// If build position available and not invalid (returns x > 1000)
		if (buildTilePosition != TilePositions::None && buildTilePosition != TilePositions::Invalid)
		{
			return buildTilePosition;
		}
	}
	return TilePositions::None;
}

bool canBuildHere(UnitType building, TilePosition buildTilePosition, bool ignoreCond)
{
	// TEMPORARY CHANGES:
	// +1 and -1 on end/start
	// mod 2 x mod 3 y
	// Start at one tile vertically above the build site and check the tile width and height + 1 to make sure units can move past and dont get stuck
	
	int offset = 0;
	if (!ignoreCond && (buildTilePosition.x % 3 == 0 || buildTilePosition.x % 2 == 0 || buildTilePosition.y % 2 == 0))
	{
		return false;
	}

	if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Pylon) == 0)
	{
		offset = 2;
	}

	for (int x = buildTilePosition.x - offset; x <= buildTilePosition.x + building.tileWidth() + offset; x++)
	{
		for (int y = buildTilePosition.y - offset; y <= buildTilePosition.y + building.tileHeight() + offset; y++)
		{
			// If the location is outside the boundaries, return false
			if (x < 0 || x > Broodwar->mapWidth() || y < 0 || y > Broodwar->mapHeight() || Broodwar->isBuildable(TilePosition(x, y), true) == false)
			{
				return false;
			}
			if (!ignoreCond)
			{
				// If the spot is not buildable, has a building on it or is within 2 tiles of a mineral field, return false
				if (mineralHeatmap[x][y] > 0 || building == UnitTypes::Protoss_Pylon && Broodwar->getUnitsInRadius(x * 32, y * 32, 128, Filter::GetType == UnitTypes::Protoss_Pylon).size() > 1)
				{
					return false;
				}
			}
		}
	}
	// If building is on an expansion tile, don't build there	
	for (auto base : nextExpansion)
	{
		for (int i = 0; i <= building.tileWidth() + 1; i++)
		{
			for (int j = 0; j <= building.tileHeight() + 1; j++)
			{
				// If the x value of this tile of the building is within an expansion, return false
				if (buildTilePosition.x + i >= base.x && buildTilePosition.x + i <= base.x + 4 && buildTilePosition.y + j >= base.y && buildTilePosition.y + j <= base.y + 3)
				{
					return false;
				}
			}
		}
	}
	// If the building site has power for buildings that need it (all except nexus/pylon), return true
	if (building == UnitTypes::Protoss_Pylon || Broodwar->hasPower(buildTilePosition, building) == true)
	{
		return true;
	}
	// If we missed anything, return false and try again	
	return false;
}

TilePosition getBuildLocationNear(UnitType building, TilePosition buildTilePosition, bool ignoreCond)
{
	int x = buildTilePosition.x;
	int y = buildTilePosition.y;
	int length = 1;
	int j = 0;
	bool first = true;
	int dx = 0;
	int dy = 1;
	// Searches in a spiral around the specified tile position (usually a nexus)
	while (length < 50)
	{
		//If we can build here, return this tile position
		if (x > 0 && x < Broodwar->mapWidth() && y > 0 && y < Broodwar->mapHeight())
		{
			if (canBuildHere(building, TilePosition(x, y), ignoreCond) == true)
			{
				return TilePosition(x, y);
			}
		}
		//Otherwise, move to another position
		x = x + dx;
		y = y + dy;
		//Count how many steps we take in this direction
		j++;
		if (j == length) //if we've reached the end, its time to turn
		{
			//reset step counter
			j = 0;

			//Increment step counter
			if (!first)
				length++;

			//First=true for every other turn so we spiral out at the right rate
			first = !first;

			//Turn counter clockwise 90 degrees:
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
	return TilePositions::None;
}

