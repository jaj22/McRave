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
			// Debug drawing
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
			nexusGrid[x][y] = 0;
		}
	}
	for (int x = 0; x <= Broodwar->mapWidth() * 4; x++)
	{
		for (int y = 0; y <= Broodwar->mapHeight() * 4; y++)
		{
			// Debug drawing
			if (antiMobilityMiniGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Broodwar->self()->getColor());
			}

			if (mobilityMiniGrid[x][y] >= 0 && antiMobilityMiniGrid[x][y] == 0)
			{
				/*	if (mobilityMiniGrid[x][y] < 4)
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
			if (enemyGroundDistanceGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Broodwar->enemy()->getColor());
			}

			antiMobilityMiniGrid[x][y] = 0;
			enemyGroundStrengthMiniGrid[x][y] = 0;
			enemyGroundDistanceGrid[x][y] = 0;
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
	for (auto u : Broodwar->self()->getUnits())
	{
		if (u->exists())
		{
			int startX = 0;
			int startY = 0;
			int offset = 0;
			if (u->getType().isBuilding())
			{
				startX = (u->getTilePosition().x * 4);
				startY = (u->getTilePosition().y * 4);
				for (int x = startX; x < startX + u->getType().tileWidth() * 4; x++)
				{
					for (int y = startY; y < startY + u->getType().tileHeight() * 4; y++)
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
		if (u.second.getDeadFrame() == 0 && !u.second.getType().isWorker() && !u.second.getType().isBuilding() && u.second.getType() != UnitTypes::Protoss_Arbiter && u.second.getType() != UnitTypes::Protoss_Observer)
		{
			for (int x = unitTilePosition.x - 5; x <= unitTilePosition.x + 6; x++)
			{
				for (int y = unitTilePosition.y - 5; y <= unitTilePosition.y + 6; y++)
				{
					if (TilePosition(x, y).isValid() && (u.second.getPosition() + Position(offsetX, offsetY)).getDistance(Position((x * 32 + offsetX), (y * 32 + offsetY))) <= 160)
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
					if (TilePosition(x, y).isValid() && (u.second.getPosition() + Position(offsetX, offsetY)).getDistance(Position((x * 32 + offsetX), (y * 32 + offsetY))) <= (range * 32))
					{
						enemyGroundStrengthGrid[x][y] += u.second.getStrength();
					}
				}
			}
		}
		if (u.second.getType().airWeapon().damageAmount() > 0 && u.second.getDeadFrame() == 0)
		{
			int range = (u.second.getType().airWeapon().maxRange()) / 32;
			// The + 1 is because we need to still check an additional tile
			for (int x = unitTilePosition.x - range; x <= unitTilePosition.x + range + 1; x++)
			{
				for (int y = unitTilePosition.y - range; y <= unitTilePosition.y + range + 1; y++)
				{
					if (TilePosition(x, y).isValid() && u.second.getPosition().getDistance(Position((x * 32), (y * 32))) <= (range * 32))
					{
						enemyAirStrengthGrid[x][y] += u.second.getStrength();
					}
				}
			}
		}
		if (u.first && u.first->exists() && !u.second.getType().isBuilding() && !u.first->isStasised() && !u.first->isMaelstrommed())
		{
			// Cluster heatmap for psi/stasis (96x96)			
			for (int x = unitTilePosition.x - 1; x <= unitTilePosition.x + 1; x++)
			{
				for (int y = unitTilePosition.y - 1; y <= unitTilePosition.y + 1; y++)
				{
					if (TilePosition(x, y).isValid())
					{
						if (!u.second.getType().isFlyer())
						{
							enemyGroundClusterGrid[x][y] += 1;
						}
						else
						{
							enemyAirClusterGrid[x][y] += 1;
						}
						if (u.second.getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode || u.second.getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode)
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
			if (!u.second.getType().isBuilding() && miniRange < 10)
			{
				miniRange = 20;
			}
			for (int x = u.second.getMiniTile().x - 50; x <= 2 + u.second.getMiniTile().x + 50; x++)
			{
				for (int y = u.second.getMiniTile().y - 50; y <= 2 + u.second.getMiniTile().y + 50; y++)
				{
					if (WalkPosition(x, y).isValid())
					{
						if (Position(x * 8, y * 8).getDistance(u.second.getPosition()) < 320)
						{
							enemyGroundDistanceGrid[x][y] += u.second.getPosition().getDistance(Position(x * 8, y * 8));
						}
						if (Position(x * 8, y * 8).getDistance(u.second.getPosition()) - u.second.getType().width() / 2 < miniRange * 8)
						{
							enemyGroundStrengthMiniGrid[x][y] += u.second.getStrength();
						}
					}					
				}
			}
		}
		if (u.second.getType().isBuilding())
		{
			WalkPosition start = u.second.getMiniTile();

			for (int x = start.x; x < start.x + u.second.getType().tileWidth() * 4; x++)
			{
				for (int y = start.y; y < start.y + u.second.getType().tileHeight() * 4; y++)
				{
					antiMobilityMiniGrid[x][y] = 1;
				}
			}
		}
		else
		{
			for (int x = u.second.getMiniTile().x; x <= u.second.getMiniTile().x + u.second.getType().tileWidth() * 4; x++)
			{
				for (int y = u.second.getMiniTile().y; y <= u.second.getMiniTile().y + u.second.getType().tileHeight() * 4; y++)
				{
					if (WalkPosition(x, y).isValid())
					{
						antiMobilityMiniGrid[x][y] = 1;
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
		for (int x = m.second.getTilePosition().x - 2; x <= m.second.getTilePosition().x + m.second.getType().tileWidth() + 2; x++)
		{
			for (int y = m.second.getTilePosition().y - 2; y <= m.second.getTilePosition().y + m.second.getType().tileHeight() + 2; y++)
			{
				if (TilePosition(x,y).isValid() && m.second.getPosition().getDistance(m.second.getClosestNexus()->getPosition()) > Position(x * 32, y * 32).getDistance(m.second.getClosestNexus()->getPosition()))
				{
					resourceGrid[x][y] = 1;
				}
			}
		}
	for (auto g : ResourceTracker::Instance().getMyGas())
	{
		// Update resource grid
		for (int x = g.second.getTilePosition().x - 1; x <= g.second.getTilePosition().x + g.second.getType().tileWidth() + 1; x++)
		{
			for (int y = g.second.getTilePosition().y - 1; y <= g.second.getTilePosition().y + g.second.getType().tileHeight() + 1; y++)
			{
				if (TilePosition(x, y).isValid() && g.second.getPosition().getDistance(g.second.getClosestNexus()->getPosition()) > Position(x * 32, y * 32).getDistance(g.second.getClosestNexus()->getPosition()))
				{
					resourceGrid[x][y] = 1;
				}
			}
		}
	}
	for (auto u : Broodwar->neutral()->getUnits())
	{
		int startX = (u->getTilePosition().x * 4);
		int startY = (u->getTilePosition().y * 4);
		for (int x = startX; x < startX + u->getType().tileWidth() * 4; x++)
		{
			for (int y = startY; y < startY + u->getType().tileHeight() * 4; y++)
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
				if (WalkPosition(x,y).isValid() && theMap.GetMiniTile(WalkPosition(x, y)).Walkable())
				{
					for (int i = -12; i <= 12; i++)
					{
						for (int j = -12; j <= 12; j++)
						{
							// The more tiles around x,y that are walkable, the more mobility x,y has				
							if (WalkPosition(x+i,y+j).isValid() && theMap.GetMiniTile(WalkPosition(x + i, y + j)).Walkable())
							{
								mobilityMiniGrid[x][y] += 1;
							}
						}
					}
					// Shrink to ratio out of 10						
					mobilityMiniGrid[x][y] = int(double(mobilityMiniGrid[x][y]) / 56);

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

void GridTrackerClass::updateObserverMovement(Unit observer)
{
	int initialx = SpecialUnitTracker::Instance().getMyObservers()[observer].getDestination().x / 32;
	int initialy = SpecialUnitTracker::Instance().getMyObservers()[observer].getDestination().y / 32;

	for (int x = initialx - 5; x <= initialx + 5; x++)
	{
		for (int y = initialy - 5; y <= initialy + 5; y++)
		{
			// Create a circle of detection rather than a square
			if (TilePosition(x,y).isValid() && Position(SpecialUnitTracker::Instance().getMyObservers()[observer].getDestination()).getDistance(Position(x * 32, y * 32)) < 160)
			{
				observerGrid[x][y] = 1;
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
				if (TilePosition(x, y).isValid() && (Position(offsetX, offsetY) + u.second.getPosition()).getDistance(Position(x * 32, y * 32)) < 288)
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
			if (WalkPosition(x, y).isValid())
			{
				antiMobilityMiniGrid[x][y] = 1;
			}
		}
	}
}