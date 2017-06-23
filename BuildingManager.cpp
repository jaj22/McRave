#include "McRave.h"

TilePosition BuildingTrackerClass::getBuildLocationNear(UnitType building, TilePosition buildTilePosition, bool ignoreCond = false)
{
	int x = buildTilePosition.x;
	int y = buildTilePosition.y;
	int length = 1;
	int j = 0;
	bool first = true;
	int dx = 0;
	int dy = 1;

	// Searches in a spiral around the specified tile position
	while (length < 50)
	{
		// If we can build here, return this tile position		
		if (TilePosition(x, y).isValid() && Util().canBuildHere(building, TilePosition(x, y), ignoreCond) == true)
		{
			return TilePosition(x, y);
		}

		// Otherwise spiral out and find a new tile
		x = x + dx;
		y = y + dy;
		j++;
		if (j == length)
		{
			j = 0;
			if (!first)
				length++;
			first = !first;
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
	Broodwar << "Out of tiles" << endl;
	return TilePositions::None;
}

TilePosition BuildingTrackerClass::getBuildLocation(UnitType building)
{
	// For each expansion, check if you can build near it, starting at the main
	for (TilePosition tile : Terrain().getActiveExpansion())
	{
		// First make sure every expansion has a pylon
		if (building == UnitTypes::Protoss_Pylon)
		{
			for (auto base : Bases().getMyBases())
			{
				if (Grids().getPylonGrid(base.second.getTilePosition()) == 0)
				{
					return getBuildLocationNear(building, base.second.getTilePosition());
				}
			}
		}

		if (building == UnitTypes::Protoss_Assimilator || building == UnitTypes::Terran_Refinery)
		{
			for (auto gas : Resources().getMyGas())
			{
				if (gas.second.getUnitType() == UnitTypes::Resource_Vespene_Geyser)
				{
					return gas.second.getTilePosition();
				}
			}
		}
		else if (building == UnitTypes::Protoss_Nexus)
		{
			for (auto base : Terrain().getNextExpansion())
			{
				if (Broodwar->getUnitsInRadius(Position(base), 128, Filter::IsResourceDepot).size() <= 0)
				{
					return base;
				}
			}
		}
		else if (building == UnitTypes::Protoss_Shield_Battery)
		{
			for (auto base : Bases().getMyBases())
			{
				if (Broodwar->getUnitsInRadius(Position(base.second.getDefensePosition()), 320, Filter::GetType == UnitTypes::Protoss_Shield_Battery).size() <= 0)
				{
					return getBuildLocationNear(building, base.second.getDefensePosition(), true);
				}
			}
		}
		else
		{
			return getBuildLocationNear(building, tile);
		}
	}
	return TilePositions::None;
}

void BuildingTrackerClass::update()
{
	queueBuildings();
	constructBuildings();
}

void BuildingTrackerClass::queueBuildings()
{
	// For each building desired
	for (auto &b : BuildOrder().getBuildingDesired())
	{
		// If our visible count is lower than our desired count
		if (b.second > Broodwar->self()->visibleUnitCount(b.first) && queuedBuildings.find(b.first) == queuedBuildings.end())
		{
			TilePosition here = Buildings().getBuildLocation(b.first);
			Unit builder = Workers().getClosestWorker(Position(here));			
		
			// If the Tile Position and Builder are valid
			if (here.isValid() && builder)
			{
				// Queue at this building type a pair of building placement and builder
				queuedBuildings.emplace(b.first, make_pair(here, builder));
				Grids().updateReservedLocation(b.first, here);
			}
		}
	}
}

void BuildingTrackerClass::constructBuildings()
{
	// Queued minerals for buildings needed
	queuedMineral = 0, queuedGas = 0;
	for (auto &b : queuedBuildings)
	{		
		queuedMineral += b.first.mineralPrice();
		queuedGas += b.first.gasPrice();

		// If worker died, replace the worker
		if (!b.second.second || !b.second.second->exists())
		{
			b.second.second = Workers().getClosestWorker(Position(b.second.first));
			continue;
		}

		// If placement is invalid, replace
		if (!b.second.first.isValid())
		{
			b.second.first = Buildings().getBuildLocation(b.first);
			continue;
		}		

		// If the worker is mining a boulder, replace
		if (b.second.second->getTarget() && b.second.second->getTarget()->getType().isMineralField() && b.second.second->getTarget()->getResources() == 0)
		{
			continue;
		}

		// If drawing is on, draw a box around the build position			
		Broodwar->drawLineMap(Position(b.second.first), b.second.second->getPosition(), Broodwar->self()->getColor());
		Broodwar->drawBoxMap(Position(b.second.first), Position(TilePosition(b.second.first.x + b.first.tileWidth(), b.second.first.y + b.first.tileHeight())), Broodwar->self()->getColor());

		// If we issued a command to this worker already, skip
		if (b.second.second->isConstructing() || b.second.second->getLastCommandFrame() >= Broodwar->getFrameCount())
		{
			continue;
		}

		// If we almost have enough resources, move the worker to the build position
		if (Broodwar->self()->minerals() >= 0.8*b.first.mineralPrice() && Broodwar->self()->minerals() <= b.first.mineralPrice() && Broodwar->self()->gas() >= 0.8*b.first.gasPrice() && Broodwar->self()->gas() <= b.first.gasPrice() || (b.second.second->getDistance(Position(b.second.first)) > 160 && Broodwar->self()->minerals() >= 0.8*b.first.mineralPrice() && 0.8*Broodwar->self()->gas() >= b.first.gasPrice()))
		{
			if (b.second.second->getOrderTargetPosition() != Position(b.second.first))
			{			
				b.second.second->move(Position(b.second.first));
			}
			continue;
		}

		// If can't build here right now and the tile is visible, replace the building position
		for (int x = b.second.first.x; x <= b.second.first.x + b.first.tileWidth(); x++)
		{
			for (int y = b.second.first.y; y <= b.second.first.y + b.first.tileHeight(); y++)
			{
				if (Broodwar->isVisible(TilePosition(x, y)) && (Grids().getReserveGrid(x, y) > 0 || !Broodwar->isBuildable(TilePosition(x, y))))
				{
					b.second.first = Buildings().getBuildLocation(b.first);
					continue;
				}
			}
		}

		// If worker is not currently returning minerals or constructing, the build position is valid and can afford the building, then proceed with build
		if (b.second.first != TilePositions::None && Broodwar->self()->minerals() >= b.first.mineralPrice() && Broodwar->self()->gas() >= b.first.gasPrice())
		{			
			b.second.second->build(b.first, b.second.first);
			continue;
		}
	}
}

void BuildingTrackerClass::updateQueue(Unit building)
{
	// When a building is created, remove from queue
	if (building->getPlayer() == Broodwar->self())
	{
		queuedBuildings.erase(building->getType());
	}
}

void BuildingTrackerClass::storeBuilding(Unit building)
{
	if (myBuildings.find(building) != myBuildings.end())
	{
		myBuildings[building].setUnit(building);
		myBuildings[building].setUnitType(building->getType());
		myBuildings[building].setPosition(building->getPosition());
		myBuildings[building].setWalkPosition(Util().getWalkPosition(building));
		myBuildings[building].setTilePosition(building->getTilePosition());
	}
	myBuildings[building].setIdleStatus(building->getRemainingTrainTime() == 0);
	myBuildings[building].setEnergy(building->getEnergy());
}

void BuildingTrackerClass::removeBuilding(Unit building)
{
	myBuildings.erase(building);
}