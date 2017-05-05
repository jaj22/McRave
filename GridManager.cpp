#include "GridManager.h"

void GridTrackerClass::reset()
{
	double strongest = 0.0;
	// For each tile, draw the current threat onto the tile
	for (int x = 0; x <= Broodwar->mapWidth(); x++)
	{
		for (int y = 0; y <= Broodwar->mapHeight(); y++)
		{
			if (enemyGroundStrengthGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%.2f", enemyGroundStrengthGrid[x][y]);
			}
			if (shuttleHeatmap[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", shuttleHeatmap[x][y]);
			}
			if (allyClusterGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%.2f", allyClusterGrid[x][y]);
			}
			if (enemyGroundClusterGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", enemyGroundClusterGrid[x][y]);
			}
			if (enemyAirClusterGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", enemyAirClusterGrid[x][y]);
			}
			if (resourceGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", resourceGrid[x][y]);
			}
			if (allyDetectorGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", allyDetectorGrid[x][y]);
			}

			if (allyClusterGrid[x][y] > strongest)
			{
				//supportPosition = Position(x * 32, y * 32);
				strongest = allyClusterGrid[x][y];
			}

			// Reset strength grids
			enemyGroundStrengthGrid[x][y] = 0;
			enemyAirStrengthGrid[x][y] = 0;

			// Reset cluster grids
			enemyGroundClusterGrid[x][y] = 0;
			enemyAirClusterGrid[x][y] = 0;
			allyClusterGrid[x][y] = 0;

			// Reset other grids
			allyDetectorGrid[x][y] = 0;
		}
	}
}