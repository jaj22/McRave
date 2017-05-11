#include "GridManager.h"
#include "UnitManager.h"
#include "TerrainManager.h"
#include "ResourceManager.h"

bool doOnce = true;

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
			if (mobilityGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", mobilityGrid[x][y]);
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
			allyPositionGrid[x][y] = 0;
		}
	}
}

void GridTrackerClass::update()
{
	reset();
	updateAllyGrids();
	updateEnemyGrids();
	updateNeutralGrids();
	updateMobilityGrids();
}

void GridTrackerClass::updateAllyGrids()
{
	for (auto &u : UnitTracker::Instance().getMyUnits())
	{
		TilePosition unitTilePosition = TilePosition(u.second.getPosition());
		int offsetX = u.second.getPosition().x % 32;
		int offsetY = u.second.getPosition().y % 32;

		// Ally cluster
		if (!u.second.getUnitType().isWorker() && !u.second.getUnitType().isBuilding())
		{
			for (int x = unitTilePosition.x - 5; x <= unitTilePosition.x + 6; x++)
			{
				for (int y = unitTilePosition.y - 5; y <= unitTilePosition.y + 6; y++)
				{
					if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && (u.second.getPosition() + Position(offsetX, offsetY)).getDistance(Position((x * 32 + offsetX), (y * 32 + offsetY))) <= 160)
					{
						allyClusterGrid[x][y] += 1;
					}
				}
			}
		}

		// Ally detection
		if (u.second.getUnitType() == UnitTypes::Protoss_Observer)
		{
			for (int x = unitTilePosition.x - 5; x <= unitTilePosition.x + 6; x++)
			{
				for (int y = unitTilePosition.y - 5; y <= unitTilePosition.y + 6; y++)
				{
					if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && (u.second.getPosition() + Position(offsetX, offsetY)).getDistance(Position((x * 32 + offsetX), (y * 32 + offsetY))) <= 160)
					{
						allyDetectorGrid[x][y] += 1;
					}
				}
			}
		}
	}
}

void GridTrackerClass::updateEnemyGrids()
{
	for (auto &u : UnitTracker::Instance().getEnUnits())
	{
		// For each enemy unit, add its attack value to each tile it is in range of
		TilePosition unitTilePosition = TilePosition(u.second.getPosition());
		if (u.second.getMaxStrength() > 1.0 && u.second.getDeadFrame() == 0)
		{
			// Store range in class
			int offsetX = u.second.getPosition().x % 32;
			int offsetY = u.second.getPosition().y % 32;
			int range = (int)u.second.getRange();
			// Making sure we properly analyze the threat of melee units without adding range to ranged units
			if (range < 32)
			{
				range = (range + 64) / 32;
			}
			else
			{
				range = range / 32;
			}
			// The + 1 is because we need to still check an additional tile
			for (int x = unitTilePosition.x - range; x <= unitTilePosition.x + range + 1; x++)
			{
				for (int y = unitTilePosition.y - range; y <= unitTilePosition.y + range + 1; y++)
				{
					if ((u.second.getPosition() + Position(offsetX, offsetY)).getDistance(Position((x * 32 + offsetX), (y * 32 + offsetY))) <= (range * 32) && x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight())
					{
						enemyGroundStrengthGrid[x][y] += u.second.getStrength();
					}
				}
			}
		}
		if (u.second.getUnitType().airWeapon().damageAmount() > 0)
		{
			int range = (u.second.getUnitType().airWeapon().maxRange()) / 32;
			// The + 1 is because we need to still check an additional tile
			for (int x = unitTilePosition.x - range; x <= unitTilePosition.x + range + 1; x++)
			{
				for (int y = unitTilePosition.y - range; y <= unitTilePosition.y + range + 1; y++)
				{
					if (u.second.getPosition().getDistance(Position((x * 32), (y * 32))) <= (range * 32) && x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight())
					{
						enemyAirStrengthGrid[x][y] += u.second.getStrength();
					}
				}
			}
		}
		if (u.first && u.first->exists() && !u.second.getUnitType().isBuilding() && !u.first->isStasised() && !u.first->isMaelstrommed())
		{
			// Cluster heatmap for psi/stasis (96x96)			
			for (int x = unitTilePosition.x - 1; x <= unitTilePosition.x + 1; x++)
			{
				for (int y = unitTilePosition.y - 1; y <= unitTilePosition.y + 1; y++)
				{
					if ((x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight()))
					{
						if (!u.second.getUnitType().isFlyer())
						{
							enemyGroundClusterGrid[x][y] += 1;
						}
						else
						{
							enemyAirClusterGrid[x][y] += 1;
						}
						if (u.second.getUnitType() == UnitTypes::Terran_Siege_Tank_Tank_Mode || u.second.getUnitType() == UnitTypes::Terran_Siege_Tank_Siege_Mode)
						{
							tankClusterGrid[x][y] += 1;
						}
					}
				}
			}
		}
	}
}

void GridTrackerClass::updateNeutralGrids()
{
	for (auto m : ResourceTracker::Instance().getMyMinerals())
		// Update resource grid
		for (int x = m.second.getTilePosition().x - 2; x <= m.second.getTilePosition().x + m.second.getUnitType().tileWidth() + 2; x++)
		{
			for (int y = m.second.getTilePosition().y - 2; y <= m.second.getTilePosition().y + m.second.getUnitType().tileHeight() + 2; y++)
			{
				if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && m.second.getPosition().getDistance(m.second.getClosestNexus()->getPosition()) > Position(x * 32, y * 32).getDistance(m.second.getClosestNexus()->getPosition()))
				{
					resourceGrid[x][y] = 1;
				}
			}
		}
	for (auto g : ResourceTracker::Instance().getMyGas())
	{
		// Update resource grid
		for (int x = g.second.getTilePosition().x - 1; x <= g.second.getTilePosition().x + g.second.getUnitType().tileWidth() + 1; x++)
		{
			for (int y = g.second.getTilePosition().y - 1; y <= g.second.getTilePosition().y + g.second.getUnitType().tileHeight() + 1; y++)
			{
				if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && g.second.getPosition().getDistance(g.second.getClosestNexus()->getPosition()) > Position(x * 32, y * 32).getDistance(g.second.getClosestNexus()->getPosition()))
				{
					resourceGrid[x][y] = 1;
				}
			}
		}
	}
}

void GridTrackerClass::updateMobilityGrids()
{
	if (doOnce)
	{
		Broodwar << "Test" << endl;
		doOnce = false;
		for (int x = 0; x <= Broodwar->mapWidth(); x++)
		{
			for (int y = 0; y <= Broodwar->mapHeight(); y++)
			{
				// If the tile has mobility
				if (theMap.GetTile(TilePosition(x, y)).Walkable())
				{
					mobilityGrid[x][y] += 1;
					for (int i = -1; i <= 1; i++)
					{
						for (int j = -1; j <= 1; j++)
						{
							// Give other tiles with mobility an increased score
							if (theMap.GetTile(TilePosition(x + i, y + j)).Walkable())
							{
								mobilityGrid[x + i][y + j] += 1;
							}
						}
					}
				}
			}
		}
	}
	/*for (auto u : UnitTracker::Instance().getMyUnits())
	{
		allyPositionGrid[TilePosition(u.second.getPosition()).x][TilePosition(u.second.getPosition()).y] += 1;
	}*/
}