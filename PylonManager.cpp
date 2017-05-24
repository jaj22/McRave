#include "PylonManager.h"

void PylonTrackerClass::update()
{
	for (auto & u : Broodwar->self()->getUnits())
	{
		if (u->isCompleted() && u->getType() == UnitTypes::Protoss_Pylon)
		{
			addToGrid(u);
		}
	}
}

void PylonTrackerClass::addToGrid(Unit unit)
{
	TilePosition pylonTile = unit->getTilePosition();
	for (int x = 0; x <= 15; ++x)
	{
		for (int y = 0; y <= 9; ++y)
		{
			bool inRangeSmall = false;
			bool inRangeMedium = false;
			bool inRangeLarge = false;

			switch (y)
			{
			case 0:
				if (x >= 4 && x <= 9)
					inRangeLarge = true;
				break;
			case 1:
			case 8:
				if (x >= 2 && x <= 13)
				{
					inRangeSmall = true;
					inRangeMedium = true;
				}
				if (x >= 1 && x <= 12)
					inRangeLarge = true;
				break;
			case 2:
			case 7:
				if (x >= 1 && x <= 14)
				{
					inRangeSmall = true;
					inRangeMedium = true;
				}
				if (x <= 13)
					inRangeLarge = true;
				break;
			case 3:
			case 4:
			case 5:
			case 6:
				if (x >= 1)
					inRangeSmall = true;
				inRangeMedium = true;
				if (x <= 14)
					inRangeLarge = true;
				break;
			case 9:
				if (x >= 5 && x <= 10)
				{
					inRangeSmall = true;
					inRangeMedium = true;
				}
				if (x >= 4 && x <= 9)
					inRangeLarge = true;
				break;
			}

			TilePosition tile = TilePosition(pylonTile.x + x - 8, pylonTile.y + y - 5);

			if (inRangeSmall)
			{
				smallPowerSites[tile] = 1;
			}

			if (inRangeMedium)
			{
				mediumPowerSites[tile] = 1;
			}

			if (inRangeLarge)
			{
				largePowerSites[tile] = 1;
			}
		}
	}	
}

bool PylonTrackerClass::hasPower(TilePosition here, UnitType building)
{
	if (building.tileHeight() == 2 && building.tileWidth() == 2 && smallPowerSites[here] != 0)
	{
		return true;
	}
	else if (building.tileHeight() == 2 && building.tileWidth() == 3 && mediumPowerSites[here] != 0)
	{		
		return true;
	}
	else if (building.tileHeight() == 3 && building.tileWidth() == 4 && largePowerSites[here] != 0)
	{
		return true;
	}
	return false;
}