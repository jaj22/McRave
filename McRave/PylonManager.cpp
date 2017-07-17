#include "McRave.h"

void PylonTrackerClass::storePylon(Unit unit)
{
	updatePower(unit);
	return;
}

void PylonTrackerClass::updatePower(Unit unit)
{
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
				if (unit->exists())
				{
					smallLocations[tile] += 1;
				}
				else
				{
					smallLocations[tile] -= 1;
				}
			}

			if (inRangeMedium)
			{
				if (unit->exists())
				{
					mediumLocations[tile] += 1;
				}
				else
				{
					mediumLocations[tile] -= 1;
				}
			}

			if (inRangeLarge)
			{
				if (unit->exists())
				{
					largeLocations[tile] += 1;
				}
				else
				{
					largeLocations[tile] -= 1;
				}
			}

		}
	}
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