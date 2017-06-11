#include "GridManager.h"
#include "UnitManager.h"
#include "TerrainManager.h"
#include "ResourceManager.h"
#include "SpecialUnitManager.h"
#include "BuildOrder.h"
#include "NexusManager.h"
#include "ProbeManager.h"
#include "BuildingManager.h"
#include <ctime>

bool doOnce = true;

void GridTrackerClass::reset()
{
	// For each tile, draw the current threat onto the tile
	int center = 0;
	for (int x = 0; x <= Broodwar->mapWidth(); x++)
	{
		for (int y = 0; y <= Broodwar->mapHeight(); y++)
		{
			// Debug drawing			
			if (eGroundClusterGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", enemyGroundClusterGrid[x][y]);
			}
			if (eAirClusterGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", enemyAirClusterGrid[x][y]);
			}
			if (resourceGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 32, y * 32), Position(x * 32 + 32, y * 32 + 32), Colors::White);
			}
			if (reserveGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 32, y * 32), Position(x*32 + 32, y*32 + 32), Colors::Black);
			}
			if (nexusGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 32, y * 32), Position(x*32 + 32, y*32 + 32), Colors::Black);
			}
			if (pylonGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 32, y * 32), Position(x * 32 + 32, y * 32 + 32), Colors::Black);
			}

			// Reset cluster grids
			eGroundClusterGrid[x][y] = 0;
			eAirClusterGrid[x][y] = 0;

			// Reset other grids
			reserveGrid[x][y] = 0;
			nexusGrid[x][y] = 0;
			pylonGrid[x][y] = 0;
			batteryGrid[x][y] = 0;
		}
	}
	for (int x = 0; x <= Broodwar->mapWidth() * 4; x++)
	{
		for (int y = 0; y <= Broodwar->mapHeight() * 4; y++)
		{
			// Debug drawing
			if (antiMobilityGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Broodwar->self()->getColor());
			}

			if (mobilityGrid[x][y] >= 0 && antiMobilityGrid[x][y] == 0)
			{
				/*if (mobilityGrid[x][y] < 4)
				{
				Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Black);
				}
				else if (mobilityGrid[x][y] >= 4 && mobilityGrid[x][y] < 7)
				{
				Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Red);
				}
				else if (mobilityGrid[x][y] >= 7 && mobilityGrid[x][y] < 10)
				{
				Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Blue);
				}
				else if (mobilityGrid[x][y] >= 10)
				{
				Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Green);
				}*/
			}

			if (eDistanceGrid[x][y] >= 0)
			{
				/*if (enemyGroundDistanceGrid[x][y] < 4)
				{
				Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Black);
				}
				else if (enemyGroundDistanceGrid[x][y] >= 4 && enemyGroundDistanceGrid[x][y] < 7)
				{
				Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Red);
				}
				else if (enemyGroundDistanceGrid[x][y] >= 7 && enemyGroundDistanceGrid[x][y] < 10)
				{
				Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Blue);
				}
				else if (enemyGroundDistanceGrid[x][y] >= 10)
				{
				Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Green);
				}*/
			}

			/*if (distanceGridHome[x][y] > 0)
			{
			if (distanceGridHome[x][y] < 50)
			{
			Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Black);
			}
			else if (distanceGridHome[x][y] >= 50 && distanceGridHome[x][y] < 100)
			{
			Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Red);
			}
			else if (distanceGridHome[x][y] >= 100 && distanceGridHome[x][y] < 150)
			{
			Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Blue);
			}
			else if (distanceGridHome[x][y] >= 150)
			{
			Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::Green);
			}
			}
			else if (distanceGridHome[x][y] < 0)
			{
			Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Colors::White);
			}*/

			if (eGroundGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Broodwar->enemy()->getColor());
			}
			if (eAirGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Broodwar->enemy()->getColor());
			}
			if (eDistanceGrid[x][y] > 1)
			{
				//Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Broodwar->enemy()->getColor());
			}
			if (eDetectorGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Broodwar->enemy()->getColor());
			}

			if (observerGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Broodwar->self()->getColor());
			}
			if (aClusterGrid[x][y] > 0)
			{
				//Broodwar->drawBoxMap(Position(x * 8, y * 8), Position(x * 8 + 8, y * 8 + 8), Broodwar->self()->getColor());
			}

			if (aClusterGrid[x][y] > center)
			{
				center = aClusterGrid[x][y];
				armyCenter = Position(WalkPosition(x, y));
			}

			aClusterGrid[x][y] = 0;
			antiMobilityGrid[x][y] = 0;
			eGroundGrid[x][y] = 0.0;
			eAirGrid[x][y] = 0.0;
			eDistanceGrid[x][y] = 0.0;
			observerGrid[x][y] = 0;
			arbiterGrid[x][y] = 0;
			eDetectorGrid[x][y] = 0;
		}
	}
	return;
}

void GridTrackerClass::update()
{
	reset();
	updateMobilityGrids();
	updateAllyGrids();
	updateEnemyGrids();
	updateNeutralGrids();
	updateDistanceGrid();
	return;
}

void GridTrackerClass::updateAllyGrids()
{
	// Clusters and anti mobility from units
	for (auto &u : UnitTracker::Instance().getMyUnits())
	{
		WalkPosition start = u.second.getMiniTile();
		int offsetX = u.second.getPosition().x % 32;
		int offsetY = u.second.getPosition().y % 32;

		// Make sure unit is alive and not an Arbiter or Observer
		if (u.second.getDeadFrame() == 0 && u.second.getType() != UnitTypes::Protoss_Arbiter && u.second.getType() != UnitTypes::Protoss_Observer)
		{
			for (int x = start.x - 20; x <= start.x + 20 + u.second.getType().tileWidth() * 4; x++)
			{
				for (int y = start.y - 20; y <= start.y + 20 + u.second.getType().tileHeight() * 4; y++)
				{
					if (WalkPosition(x, y).isValid() && (u.second.getPosition() + Position(offsetX, offsetY)).getDistance(Position((x * 8 + offsetX), (y * 8 + offsetY))) <= 160)
					{
						aClusterGrid[x][y] += 1;
					}
				}
			}
			// Anti mobility doesn't apply to flying units (carriers, scouts, shuttles)
			if (!u.second.getType().isFlyer())
			{
				for (int x = start.x; x <= start.x + u.second.getType().tileWidth() * 4; x++)
				{
					for (int y = start.y; y <= start.y + u.second.getType().tileHeight() * 4; y++)
					{
						if (WalkPosition(x, y).isValid())
						{
							antiMobilityGrid[x][y] = 1;
						}
					}
				}
			}
		}
	}

	// Anti mobility from buildings
	for (auto &u : BuildingTracker::Instance().getMyBuildings())
	{
		if (u.first->exists())
		{
			int startX = 0;
			int startY = 0;
			int offset = 0;
			startX = (u.first->getTilePosition().x * 4);
			startY = (u.first->getTilePosition().y * 4);
			for (int x = startX - 2; x < 2 + startX + u.second.getType().tileWidth() * 4; x++)
			{
				for (int y = startY - 2; y < 2 + startY + u.second.getType().tileHeight() * 4; y++)
				{
					if (WalkPosition(x, y).isValid())
					{
						antiMobilityGrid[x][y] = 1;
					}
				}
			}

			startX = u.first->getTilePosition().x;
			startY = u.first->getTilePosition().y;

			if (u.second.getType() == UnitTypes::Protoss_Gateway || u.second.getType() == UnitTypes::Protoss_Robotics_Facility)
			{
				offset = 1;
			}
			for (int x = startX - offset; x < startX + u.second.getType().tileWidth() + offset; x++)
			{
				for (int y = startY - offset; y < startY + u.second.getType().tileHeight() + offset; y++)
				{
					if (TilePosition(x, y).isValid())
					{
						reserveGrid[x][y] = 1;
					}
				}
			}
			if (u.second.getType() == UnitTypes::Protoss_Pylon)
			{
				for (int x = u.second.getTilePosition().x - 4; x < u.second.getTilePosition().x + u.second.getType().tileWidth() + 4; x++)
				{
					for (int y = u.second.getTilePosition().y - 4; y < u.second.getTilePosition().y + u.second.getType().tileHeight() + 4; y++)
					{
						if (TilePosition(x, y).isValid())
						{
							pylonGrid[x][y] += 1;
						}
					}
				}
			}
			if (u.second.getType() == Protoss_Shield_Battery)
			{
				for (int x = u.second.getTilePosition().x - 10; x < u.second.getTilePosition().x + u.second.getType().tileWidth() + 10; x++)
				{
					for (int y = u.second.getTilePosition().y - 10; y < u.second.getTilePosition().y + u.second.getType().tileHeight() + 10; y++)
					{
						if (TilePosition(x, y).isValid() && u.first->getDistance(Position(TilePosition(x, y))) < 320)
						{
							batteryGrid[x][y] = 1;
						}
					}
				}
			}
		}
	}

	for (auto & probe : ProbeTracker::Instance().getMyProbes())
	{
		WalkPosition start = probe.second.getMiniTile();
		for (int x = start.x; x <= start.x + probe.first->getType().tileWidth() * 4; x++)
		{
			for (int y = start.y; y <= start.y + probe.first->getType().tileHeight() * 4; y++)
			{
				if (WalkPosition(x, y).isValid())
				{
					antiMobilityGrid[x][y] = 1;
				}
			}
		}
	}

	// Nexus grid 
	for (auto & nexus : NexusTracker::Instance().getMyNexus())
	{
		int offset = 8;
		for (int x = nexus.second.getTilePosition().x - offset; x < nexus.second.getTilePosition().x + 4 + offset; x++)
		{
			for (int y = nexus.second.getTilePosition().y - offset; y < nexus.second.getTilePosition().y + 3 + offset; y++)
			{
				nexusGrid[x][y] = 1;
			}
		}
	}
	return;
}

void GridTrackerClass::updateEnemyGrids()
{
	for (auto &u : UnitTracker::Instance().getEnUnits())
	{
		// Cluster grid for storm/stasis (96x96)
		TilePosition unitTilePosition = TilePosition(u.second.getPosition());
		if (u.first && u.first->exists() && !u.second.getType().isBuilding() && !u.first->isStasised() && !u.first->isMaelstrommed())
		{
			for (int x = unitTilePosition.x - 1; x <= unitTilePosition.x + 1; x++)
			{
				for (int y = unitTilePosition.y - 1; y <= unitTilePosition.y + 1; y++)
				{
					if (TilePosition(x, y).isValid())
					{
						if (!u.second.getType().isFlyer())
						{
							eGroundClusterGrid[x][y] += 1;
						}
						else
						{
							eAirClusterGrid[x][y] += 1;
						}
						if (u.second.getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode || u.second.getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode)
						{
							stasisClusterGrid[x][y] += 1;
						}
					}
				}
			}
		}

		// Detector grid
		if (u.second.getType() == UnitTypes::Protoss_Observer || u.second.getType() == UnitTypes::Protoss_Photon_Cannon || u.second.getType() == UnitTypes::Zerg_Overlord || u.second.getType() == UnitTypes::Zerg_Spore_Colony || u.second.getType() == UnitTypes::Terran_Science_Vessel || u.second.getType() == UnitTypes::Terran_Missile_Turret)
		{
			for (int x = u.second.getMiniTile().x - 40; x <= 2 + u.second.getMiniTile().x + 40; x++)
			{
				for (int y = u.second.getMiniTile().y - 40; y <= 2 + u.second.getMiniTile().y + 40; y++)
				{
					if (WalkPosition(x, y).isValid() && Position(WalkPosition(x, y)).getDistance(u.second.getPosition()) < u.second.getType().sightRange())
					{
						eDetectorGrid[x][y] = 1;
					}
				}
			}
		}

		// Distance threat grids and strength grids
		if (u.second.getDeadFrame() == 0)
		{
			for (int x = u.second.getMiniTile().x - 50; x <= 2 + u.second.getMiniTile().x + 50; x++)
			{
				for (int y = u.second.getMiniTile().y - 50; y <= 2 + u.second.getMiniTile().y + 50; y++)
				{
					if (WalkPosition(x, y).isValid())
					{
						if (Position(x * 8, y * 8).getDistance(u.second.getPosition()) < 320 && u.second.getGroundDamage() > 0.0)
						{
							if (Position(x * 8, y * 8).getDistance(u.second.getPosition()) > 0)
							{
								eDistanceGrid[x][y] += 10.0 / (1.0 + u.second.getPosition().getDistance(Position(x * 8, y * 8)) / 32);
							}
							else
							{
								eDistanceGrid[x][y] += 10.0;
							}
						}
						if (u.second.getGroundDamage() > 0.0 && Position(x * 8, y * 8).getDistance(u.second.getPosition()) - u.second.getType().tileWidth() * 16 < u.second.getGroundRange() + u.second.getSpeed() * 8)
						{
							eGroundGrid[x][y] += u.second.getMaxStrength();
						}
						if (u.second.getAirDamage() > 0.0 && Position(x * 8, y * 8).getDistance(u.second.getPosition()) - u.second.getType().tileWidth() * 16 < u.second.getAirRange() + u.second.getSpeed() * 8)
						{
							eAirGrid[x][y] += u.second.getMaxStrength();
						}
					}
				}
			}
		}

		// Anti mobility grids
		if (u.second.getType().isBuilding())
		{
			WalkPosition start = u.second.getMiniTile();

			for (int x = start.x; x < start.x + u.second.getType().tileWidth() * 4; x++)
			{
				for (int y = start.y; y < start.y + u.second.getType().tileHeight() * 4; y++)
				{
					if (WalkPosition(x, y).isValid())
					{
						antiMobilityGrid[x][y] = 1;
					}
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
						antiMobilityGrid[x][y] = 1;
					}
				}
			}
		}
	}
	return;
}

void GridTrackerClass::updateNeutralGrids()
{
	for (auto m : ResourceTracker::Instance().getMyMinerals())
	{
		// Update resource grid
		for (int x = m.second.getTilePosition().x - 5; x < m.second.getTilePosition().x + m.second.getType().tileWidth() + 5; x++)
		{
			for (int y = m.second.getTilePosition().y - 5; y < m.second.getTilePosition().y + m.second.getType().tileHeight() + 5; y++)
			{
				if (Position(NexusTracker::Instance().getMyNexus()[m.second.getClosestNexus()].getCannonPosition()).getDistance(Position(TilePosition(x, y))) <= 192 && TilePosition(x, y).isValid() && m.second.getPosition().getDistance(m.second.getClosestNexus()->getPosition()) + 64 > Position(x * 32, y * 32).getDistance(m.second.getClosestNexus()->getPosition()))
				{
					resourceGrid[x][y] = 1;
				}
			}
		}
	}
	for (auto g : ResourceTracker::Instance().getMyGas())
	{
		// Update resource grid
		for (int x = g.second.getTilePosition().x - 5; x < g.second.getTilePosition().x + g.second.getType().tileWidth() + 5; x++)
		{
			for (int y = g.second.getTilePosition().y - 5; y < g.second.getTilePosition().y + g.second.getType().tileHeight() + 5; y++)
			{
				if (Position(NexusTracker::Instance().getMyNexus()[g.second.getClosestNexus()].getCannonPosition()).getDistance(Position(TilePosition(x, y))) <= 256 && TilePosition(x, y).isValid() && g.second.getPosition().getDistance(g.second.getClosestNexus()->getPosition()) + 64 > Position(x * 32, y * 32).getDistance(g.second.getClosestNexus()->getPosition()))
				{
					resourceGrid[x][y] = 1;
				}
			}
		}
	}

	// Anti mobility around all neutral units, not just my minerals/gas
	for (auto u : Broodwar->neutral()->getUnits())
	{
		if (u->getType().isFlyer())
		{
			continue;
		}
		int startX = (u->getTilePosition().x * 4);
		int startY = (u->getTilePosition().y * 4);
		for (int x = startX - 2; x < 2 + startX + u->getType().tileWidth() * 4; x++)
		{
			for (int y = startY - 2; y < 2 + startY + u->getType().tileHeight() * 4; y++)
			{
				antiMobilityGrid[x][y] = 1;
			}
		}
	}
	return;
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
				mobilityGrid[x][y] = 0;
			}
		}
		for (int x = 0; x <= Broodwar->mapWidth() * 4; x++)
		{
			for (int y = 0; y <= Broodwar->mapHeight() * 4; y++)
			{
				if (WalkPosition(x, y).isValid() && theMap.GetMiniTile(WalkPosition(x, y)).Walkable())
				{
					for (int i = -12; i <= 12; i++)
					{
						for (int j = -12; j <= 12; j++)
						{
							// The more tiles around x,y that are walkable, the more mobility x,y has				
							if (WalkPosition(x + i, y + j).isValid() && theMap.GetMiniTile(WalkPosition(x + i, y + j)).Walkable())
							{
								mobilityGrid[x][y] += 1;
							}
						}
					}
					mobilityGrid[x][y] = int(double(mobilityGrid[x][y]) / 56);

					if (getNearestChokepoint(Position(x * 8, y * 8)) && getNearestChokepoint(Position(x * 8, y * 8))->getCenter().getDistance(Position(x * 8, y * 8)) < 320)
					{
						bool notCorner = true;
						int startRatio = int(pow(getNearestChokepoint(Position(x * 8, y * 8))->getCenter().getDistance(Position(x * 8, y * 8)) / 64, 2.0));
						for (int i = 0 - startRatio; i <= startRatio; i++)
						{
							for (int j = 0 - startRatio; j <= 0 - startRatio; j++)
							{
								if (WalkPosition(x + i, y + j).isValid() && !theMap.GetMiniTile(WalkPosition(x + i, y + j)).Walkable())
								{
									notCorner = false;
								}
							}
						}

						if (notCorner)
						{
							mobilityGrid[x][y] = 10;
						}
					}

					// Max a mini grid to 10
					mobilityGrid[x][y] = min(mobilityGrid[x][y], 10);
				}

				// Setup what is possible to check ground distances on
				if (mobilityGrid[x][y] <= 0)
				{
					distanceGridHome[x][y] = -1;
				}
				else if (mobilityGrid[x][y] > 0)
				{
					distanceGridHome[x][y] = 0;
				}
				/*if (!getRegion(TilePosition(WalkPosition(x, y))) || TerrainTracker::Instance().getIslandRegions().find(getRegion(TilePosition(WalkPosition(x, y)))) != TerrainTracker::Instance().getIslandRegions().end())
				{
				distanceGridHome[x][y] = -1;
				}*/
			}
		}
	}
	return;
}

void GridTrackerClass::updateObserverMovement(Unit observer)
{
	WalkPosition destination = WalkPosition(SpecialUnitTracker::Instance().getMyObservers()[observer].getDestination());

	for (int x = destination.x - 20; x <= destination.x + 20; x++)
	{
		for (int y = destination.y - 20; y <= destination.y + 20; y++)
		{
			// Create a circle of detection rather than a square
			if (WalkPosition(x, y).isValid() && SpecialUnitTracker::Instance().getMyObservers()[observer].getDestination().getDistance(Position(WalkPosition(x, y))) < 160)
			{
				observerGrid[x][y] = 1;
			}
		}
	}
	return;
}

void GridTrackerClass::updateArbiterMovement(Unit arbiter)
{
	WalkPosition destination = WalkPosition(SpecialUnitTracker::Instance().getMyArbiters()[arbiter].getDestination());

	for (int x = destination.x - 20; x <= destination.x + 20; x++)
	{
		for (int y = destination.y - 20; y <= destination.y + 20; y++)
		{
			// Create a circle of detection rather than a square
			if (WalkPosition(x, y).isValid() && SpecialUnitTracker::Instance().getMyArbiters()[arbiter].getDestination().getDistance(Position(WalkPosition(x, y))) < 160)
			{
				arbiterGrid[x][y] = 1;
			}
		}
	}
	return;
}

void GridTrackerClass::updateAllyMovement(Unit unit, WalkPosition here)
{
	for (int x = here.x - unit->getType().width() / 16; x <= here.x + unit->getType().width() / 16; x++)
	{
		for (int y = here.y - unit->getType().height() / 16; y <= here.y + unit->getType().height() / 16; y++)
		{
			if (WalkPosition(x, y).isValid())
			{
				antiMobilityGrid[x][y] = 1;
			}
		}
	}
	return;
}

void GridTrackerClass::updateReservedLocation(UnitType building, TilePosition here)
{
	// When placing a building, reserve the tiles so no further locations are placed there
	for (int x = here.x; x < here.x + building.tileWidth(); x++)
	{
		for (int y = here.y; y < here.y + building.tileHeight(); y++)
		{
			reserveGrid[x][y] = 1;
		}
	}
	return;
}

void GridTrackerClass::updateDistanceGrid()
{
	// TODO: Goal with this grid is to create a ground distance grid from home for unit micro
	if (TerrainTracker::Instance().getAnalyzed() && distanceOnce && Broodwar->getFrameCount() > 500)
	{
		WalkPosition start = WalkPosition(TerrainTracker::Instance().getPlayerStartingPosition());
		distanceGridHome[start.x][start.y] = 1;
		distanceOnce = false;
		bool done = false;
		int cnt = 0;
		int segment = 0;
		clock_t myClock;
		double duration;
		myClock = clock();

		while (!done)
		{
			duration = (clock() - myClock) / (double)CLOCKS_PER_SEC;
			if (duration > 2)
			{
				break;
			}
			done = true;
			cnt++;
			segment += 8;
			for (int x = 0; x <= Broodwar->mapWidth() * 4; x++)
			{
				for (int y = 0; y <= Broodwar->mapHeight() * 4; y++)
				{
					// If any of the grid is 0, we're not done yet
					if (distanceGridHome[x][y] == 0 && theMap.GetMiniTile(WalkPosition(x, y)).AreaId() > 0)
					{
						done = false;
					}
					if (distanceGridHome[x][y] == cnt)
					{
						for (int i = x - 1; i <= x + 1; i++)
						{
							for (int j = y - 1; j <= y + 1; j++)
							{
								if (distanceGridHome[i][j] == 0 && Position(WalkPosition(i, j)).getDistance(Position(start)) <= segment)
								{
									distanceGridHome[i][j] = cnt + 1;
								}
							}
						}

						/*if (distanceGridHome[x][y - 1] == 0)
						{
						distanceGridHome[x][y - 1] = cnt + 1;
						}
						if (distanceGridHome[x][y + 1] == 0)
						{
						distanceGridHome[x][y + 1] = cnt + 1;
						}
						if (distanceGridHome[x - 1][y] == 0)
						{
						distanceGridHome[x - 1][y] = cnt + 1;
						}
						if (distanceGridHome[x + 1][y] == 0)
						{
						distanceGridHome[x + 1][y] = cnt + 1;
						}*/
					}
				}
			}
		}
		Broodwar << "Distance Grid Analysis time: " << duration << endl;
		if (duration > 2)
		{
			Broodwar << "Hit maximum, check for islands." << endl;
		}
	}
}