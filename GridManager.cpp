#include "GridManager.h"
#include "UnitManager.h"
#include "TerrainManager.h"
#include "ResourceManager.h"
#include "SpecialUnitManager.h"

bool doOnce = true;

void GridTrackerClass::reset()
{
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
			if (observerGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", observerGrid[x][y]);
			}
			if (reserveGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 32, y * 32), Position(x*32 + 32, y*32 + 32), Colors::Black);
			}
			if (nexusGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 32, y * 32), Position(x*32 + 32, y*32 + 32), Colors::Black);
			}

			// Reset strength grids
			enemyGroundStrengthGrid[x][y] = 0;
			enemyAirStrengthGrid[x][y] = 0;

			// Reset cluster grids
			enemyGroundClusterGrid[x][y] = 0;
			enemyAirClusterGrid[x][y] = 0;
			allyClusterGrid[x][y] = 0;

			// Reset other grids
			observerGrid[x][y] = 0;
			reserveGrid[x][y] = 0;
		}
	}
	for (int x = 0; x <= Broodwar->mapWidth() * 4; x++)
	{
		for (int y = 0; y <= Broodwar->mapHeight() * 4; y++)
		{
			if (antiMobilityMiniGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Broodwar->self()->getColor());
			}		

			if (mobilityMiniGrid[x][y] >= 0 && antiMobilityMiniGrid[x][y] == 0)
			{
				//Broodwar->drawCircleMap(Position(x * 8 + 4, y * 8 + 4), (int)mobilityMiniGrid[x][y] / 32, Broodwar->self()->getColor());
				/*if (mobilityMiniGrid[x][y] < 4)
				{
					Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Black);
				}
				else if (mobilityMiniGrid[x][y] >= 4 && mobilityMiniGrid[x][y] < 7)
				{
					Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Red);
				}
				else if (mobilityMiniGrid[x][y] >= 7 && mobilityMiniGrid[x][y] < 10)
				{
					Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Blue);
				}
				else if (mobilityMiniGrid[x][y] >= 10)
				{
					Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Green);
				}*/
			}

			if (enemyGroundStrengthMiniGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Broodwar->enemy()->getColor());
			}

			antiMobilityMiniGrid[x][y] = 0;
			enemyGroundStrengthMiniGrid[x][y] = 0;
		}
	}
}

void GridTrackerClass::update()
{
	reset();
	updateMobilityGrids();
	updateAllyGrids();
	updateEnemyGrids();
	updateNeutralGrids();
	//updateDistanceGrid();
}

void GridTrackerClass::updateAllyGrids()
{
	// -- Anti Mobility Grid --
	// For all my units, it gets the units tile position and uses it to find the closest mini tile on which it's standing on
	// Any mini tile on which a unit is standing is considered not mobile
	// All buildings have a buffer zone of 2 mini tiles around it to prevent issues with collision on edges and All buildings have a slightly higher width and height due to BWAPI
	for (auto u : Broodwar->self()->getUnits())
	{
		if (u->exists())
		{
			int startX = 0;
			int startY = 0;
			int offset = 0;
			if (u->getType().isBuilding())
			{
				startX = (u->getTilePosition().x * 4) - 2;
				startY = (u->getTilePosition().y * 4) - 2;
				for (int x = startX; x < 4 + startX + u->getType().tileWidth() * 4; x++)
				{
					for (int y = startY; y < 4 + startY + u->getType().tileHeight() * 4; y++)
					{
						antiMobilityMiniGrid[x][y] = 1;
					}
				}

				startX = u->getTilePosition().x;
				startY = u->getTilePosition().y;

				if (u->getType() == UnitTypes::Protoss_Gateway || u->getType() == UnitTypes::Protoss_Robotics_Facility)
				{
					offset = 1;
				}
				for (int x = startX - offset; x < startX + u->getType().tileWidth() + offset; x++)
				{
					for (int y = startY - offset; y < startY + u->getType().tileHeight() + offset; y++)
					{
						reserveGrid[x][y] = 1;
					}
				}	
				if (u->getType().isResourceDepot())
				{
					offset = 10;
					for (int x = startX - offset; x < startX + u->getType().tileWidth() + offset; x++)
					{
						for (int y = startY - offset; y < startY + u->getType().tileHeight() + offset; y++)
						{
							nexusGrid[x][y] = 1;
						}
					}
				}
			}
			else
			{
				startX = (int)((u->getPosition().x - u->getPosition().x % 8 - (0.5*u->getType().width())) / 8);
				startY = (int)((u->getPosition().y - u->getPosition().y % 8 - (0.5*u->getType().height())) / 8);
				for (int x = startX; x <= startX + u->getType().tileWidth() * 4; x++)
				{
					for (int y = startY; y <= startY + u->getType().tileHeight() * 4; y++)
					{
						antiMobilityMiniGrid[x][y] = 1;
					}
				}
			}

		}
	}
	for (auto &u : UnitTracker::Instance().getMyUnits())
	{
		TilePosition unitTilePosition = TilePosition(u.second.getPosition());
		int offsetX = u.second.getPosition().x % 32;
		int offsetY = u.second.getPosition().y % 32;

		// Ally cluster grid
		if (u.second.getDeadFrame() == 0 && !u.second.getUnitType().isWorker() && !u.second.getUnitType().isBuilding() && u.second.getUnitType() != UnitTypes::Protoss_Arbiter && u.second.getUnitType() != UnitTypes::Protoss_Observer)
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
			int range = (int)u.second.getRange() + 64;
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
					if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && (u.second.getPosition() + Position(offsetX, offsetY)).getDistance(Position((x * 32 + offsetX), (y * 32 + offsetY))) <= (range * 32))
					{
						enemyGroundStrengthGrid[x][y] += u.second.getStrength();
					}
				}
			}
		}
		if (u.second.getUnitType().airWeapon().damageAmount() > 0 && u.second.getDeadFrame() == 0)
		{
			int range = (u.second.getUnitType().airWeapon().maxRange()) / 32;
			// The + 1 is because we need to still check an additional tile
			for (int x = unitTilePosition.x - range; x <= unitTilePosition.x + range + 1; x++)
			{
				for (int y = unitTilePosition.y - range; y <= unitTilePosition.y + range + 1; y++)
				{
					if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && u.second.getPosition().getDistance(Position((x * 32), (y * 32))) <= (range * 32))
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

	for (auto &u : UnitTracker::Instance().getEnUnits())
	{
		if (u.second.getDeadFrame() == 0)
		{
			int miniRange = u.second.getRange() / 8;
			if (!u.second.getUnitType().isBuilding() && miniRange < 10)
			{
				miniRange = 20;
			}
			for (int i = u.second.getMiniTile().x - miniRange - 2; i <= 2 + u.second.getMiniTile().x + miniRange; i++)
			{
				for (int j = u.second.getMiniTile().y - miniRange - 2; j <= 2 + u.second.getMiniTile().y + miniRange; j++)
				{
					if (i > 0 && i < Broodwar->mapWidth() * 4 && j > 0 && j < Broodwar->mapHeight() * 4 && Position(i * 8, j * 8).getDistance(u.second.getPosition()) - u.second.getUnitType().width() / 2 < miniRange * 8)
					{
						enemyGroundStrengthMiniGrid[i][j] += u.second.getStrength();
					}
				}
			}
		}
	}
}

void GridTrackerClass::updateNeutralGrids()
{
	double distanceTo = 0.0;
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
	for (auto u : Broodwar->neutral()->getUnits())
	{
		int startX = (u->getTilePosition().x * 4) - 2;
		int startY = (u->getTilePosition().y * 4) - 2;
		for (int x = startX; x < 4 + startX + u->getType().tileWidth() * 4; x++)
		{
			for (int y = startY; y < 4 + startY + u->getType().tileHeight() * 4; y++)
			{
				antiMobilityMiniGrid[x][y] = 1;
			}
		}
	}
}

void GridTrackerClass::updateMobilityGrids()
{
	if (doOnce && TerrainTracker::Instance().getAnalyzed())
	{
		doOnce = false;	
		for (int x = 0; x <= Broodwar->mapWidth() * 4; x++)
		{
			for (int y = 0; y <= Broodwar->mapHeight() * 4; y++)
			{
				mobilityMiniGrid[x][y] += 0;
			}
		}
		for (int x = 0; x <= Broodwar->mapWidth() * 4; x++)
		{
			for (int y = 0; y <= Broodwar->mapHeight() * 4; y++)
			{
				if (theMap.GetMiniTile(WalkPosition(x, y)).Walkable())
				{
					for (int i = -12; i <= 12; i++)
					{
						for (int j = -12; j <= 12; j++)
						{
							// The more tiles around x,y that are walkable, the more mobility x,y has				
							if (x + i >= 0 && x + i <= Broodwar->mapWidth() * 4 && y + j >= 0 && y + j <= Broodwar->mapHeight() * 4 && theMap.GetMiniTile(WalkPosition(x + i, y + j)).Walkable())
							{
								mobilityMiniGrid[x][y] += 1;
							}
						}
					}
					// Shrink to ratio out of 10						
					mobilityMiniGrid[x][y] = int(double(mobilityMiniGrid[x][y]) / 85);

					if (getNearestChokepoint(Position(x * 8, y * 8)) && getNearestChokepoint(Position(x * 8, y * 8))->getCenter().getDistance(Position(x * 8, y * 8)) < 320)
					{
						// Scale of 1-10 of how close the position is
						mobilityMiniGrid[x][y] = 10;
					}

					//Max a mini grid to 10
					mobilityMiniGrid[x][y] = min(mobilityMiniGrid[x][y], 10);
				}
			}
		}
	}
}

void GridTrackerClass::updateObserverGrids()
{
	for (auto u : SpecialUnitTracker::Instance().getMyObservers())
	{
		int initialx = TilePosition(u.second.getDestination()).x;
		int initialy = TilePosition(u.second.getDestination()).y;
		int offsetX = u.second.getPosition().x % 32;
		int offsetY = u.second.getPosition().y % 32;

		for (int x = initialx - 9; x <= initialx + 9; x++)
		{
			for (int y = initialy - 9; y <= initialy + 9; y++)
			{
				// Create a circle of detection rather than a square
				if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && (Position(offsetX, offsetY) + u.second.getPosition()).getDistance(Position(x * 32, y * 32)) < 288)
				{
					observerGrid[x][y] = 1;
				}
			}
		}
	}
}

void GridTrackerClass::updateArbiterGrids()
{
	for (auto u : SpecialUnitTracker::Instance().getMyArbiters())
	{
		int initialx = TilePosition(u.second.getDestination()).x;
		int initialy = TilePosition(u.second.getDestination()).y;
		int offsetX = u.second.getPosition().x % 32;
		int offsetY = u.second.getPosition().y % 32;

		for (int x = initialx - 9; x <= initialx + 9; x++)
		{
			for (int y = initialy - 9; y <= initialy + 9; y++)
			{
				// Create a circle of detection rather than a square
				if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && (Position(offsetX, offsetY) + u.second.getPosition()).getDistance(Position(x * 32, y * 32)) < 288)
				{
					arbiterGrid[x][y] += 1;
				}
			}
		}
	}
}

void GridTrackerClass::updateAllyMovement(Unit unit, WalkPosition here)
{
	for (int x = here.x - unit->getType().width() / 16; x <= here.x + unit->getType().width() / 16; x++)
	{
		for (int y = here.y - unit->getType().height() / 16; y <= here.y + unit->getType().height() / 16; y++)
		{
			antiMobilityMiniGrid[x][y] = 1;
		}
	}
}

void GridTrackerClass::updateDistanceGrid()
{
	// TODO: Goal with this grid is to create a ground distance grid from home for unit micro
	if (TerrainTracker::Instance().getAnalyzed() && distanceOnce)
	{
		for (int x = 0; x <= Broodwar->mapWidth() * 4; x++)
		{
			for (int y = 0; y <= Broodwar->mapHeight() * 4; y++)
			{
				distanceGridHome[x][y] = 0;
			}
		}
		int x = TerrainTracker::Instance().getPlayerStartingTilePosition().x * 4;
		int y = TerrainTracker::Instance().getPlayerStartingTilePosition().y * 4;
		int mapWidth = Broodwar->mapWidth() * 4;
		int mapHeight = Broodwar->mapHeight() * 4;
		distanceOnce = false;

		bool done = false;
		int cnt = 0;
		while (!done)
		{
			cnt++;
			for (int i = x - cnt; i <= x + cnt; i++)
			{
				for (int j = y - cnt; j <= y + cnt; j++)
				{
					if (i >= 0 && i <= mapWidth && j >= 0 && j <= mapHeight && distanceGridHome[i][j] == 0 && mobilityMiniGrid[i][j] > 0)
					{
						distanceGridHome[i][j] = cnt;
					}
				}
			}
			if (cnt >= 1024)
			{
				Broodwar << "Test" << endl;
				done = true;
			}
		}
	}
}