#include "McRave.h"

void PylonTrackerClass::storePylon(Unit unit)
{
	myPylons.emplace(unit);
	storePower(unit);
	return;
}

void PylonTrackerClass::storePower(Unit unit)
{
	if (!unit || !unit->exists() || !unit->isCompleted())
	{
		return;
	}
	TilePosition pylonTile = unit->getTilePosition();
	for (int x = 0; x <= 15; x++)
	{
		for (int y = 0; y <= 9; y++)
		{
			bool inRangeSmall = false;
			bool inRangeMedium = false;
			bool inRangeLarge = false;


			if (y == 0 && x >= 4 && x <= 9)
			{
				inRangeLarge = true;
			}
			if (y == 1 || y == 8)
			{
				if (x >= 2 && x <= 13)
				{
					inRangeSmall = true;
					inRangeMedium = true;
				}
				if (x >= 1 && x <= 12)
				{
					inRangeLarge = true;
				}
			}
			if (y == 2 || y == 7)
			{
				if (x >= 1 && x <= 14)
				{
					inRangeSmall = true;
					inRangeMedium = true;
				}
				if (x <= 13)
				{
					inRangeLarge = true;
				}
			}
			if (y == 3 || y == 4 || y == 5 || y == 6)
			{
				if (x >= 1)
				{
					inRangeSmall = true;
					inRangeMedium = true;
				}
				if (x <= 14)
				{
					inRangeLarge = true;
				}

			}
			if (y == 9)
			{
				if (x >= 5 && x <= 10)
				{
					inRangeSmall = true;
					inRangeMedium = true;
				}
				if (x >= 4 && x <= 9)
				{
					inRangeLarge = true;
				}

			}

			TilePosition tile = TilePosition(pylonTile.x + x - 8, pylonTile.y + y - 5);

			if (inRangeSmall)
			{
				smallLocations[tile] = 1;
			}

			if (inRangeMedium)
			{
				mediumLocations[tile] = 1;
			}

			if (inRangeLarge)
			{
				largeLocations[tile] = 1;
			}
		}
	}
	/*for (auto tile : mediumPowerSites)
	{
	Broodwar->drawBoxMap(Position(tile.first), Position(tile.first) + Position(32, 32), Colors::Black);
	}*/
	return;
}

bool PylonTrackerClass::hasPower(TilePosition here, UnitType building)
{
	if (building.tileHeight() == 2 && building.tileWidth() == 2 && smallLocations[here] != 0)
	{
		return true;
	}
	else if (building.tileHeight() == 2 && building.tileWidth() == 3 && mediumLocations[here] != 0)
	{
		return true;
	}
	else if (building.tileHeight() == 3 && building.tileWidth() == 4 && largeLocations[here] != 0)
	{
		return true;
	}
	return false;
}