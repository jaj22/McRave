#include "BuildOrder.h"
#include "BuildingManager.h"
#include "GridManager.h"
#include "TerrainManager.h"
#include "PylonManager.h"
#include "ResourceManager.h"
#include "ProbeManager.h"

bool canBuildHere(UnitType building, TilePosition buildTilePosition, bool ignoreCond)
{
	int offset = 0;
	// Offset for first pylon
	if (building == UnitTypes::Protoss_Pylon && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Pylon) <= 0)
	{
		if (TerrainTracker::Instance().getDefendHere()[0].getDistance(Position(buildTilePosition)) > 640)
		{
			return false;
		}
		offset = 3;
	}

	// Offset for production buildings
	if (building == UnitTypes::Protoss_Gateway || building == UnitTypes::Protoss_Robotics_Facility)
	{
		offset = 1;
	}

	// Check if it's not a pylon and in a preset buildable position based on power grid
	if (building != UnitTypes::Protoss_Pylon && !PylonTracker::Instance().hasPower(buildTilePosition, building))
	{
		return false;
	}

	// Check if it's a pylon and in a buildable position
	if (building == UnitTypes::Protoss_Pylon && !Broodwar->canBuildHere(buildTilePosition, building))
	{
		return false;
	}

	// For every tile of a buildings size
	for (int x = buildTilePosition.x; x < buildTilePosition.x + building.tileWidth(); x++)
	{
		for (int y = buildTilePosition.y; y < buildTilePosition.y + building.tileHeight(); y++)
		{
			// Checking if the tile is valid, and the reasons that could make this position an unsuitable build location
			if (TilePosition(x, y).isValid())
			{
				// If it's reserved
				if (GridTracker::Instance().getReserveGrid(x, y) > 0)
				{
					return false;
				}

				// If it's a pylon and overlapping too many pylons
				if (building == UnitTypes::Protoss_Pylon && GridTracker::Instance().getPylonGrid(x, y) >= 2)
				{
					return false;
				}

				// If it's not a cannon and on top of the resource grid
				if (building != UnitTypes::Protoss_Photon_Cannon && GridTracker::Instance().getResourceGrid(x, y) > 0)
				{
					return false;
				}

				// If it's on an unbuildable tile
				if (!Broodwar->isBuildable(TilePosition(x, y), true))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}

	// If the building requires an offset (production buildings and first pylon)
	if (offset > 0)
	{
		for (int x = buildTilePosition.x - offset; x < buildTilePosition.x + building.tileWidth() + offset; x++)
		{
			for (int y = buildTilePosition.y - offset; y < buildTilePosition.y + building.tileHeight() + offset; y++)
			{
				if (GridTracker::Instance().getReserveGrid(x, y) > 0 && !Broodwar->isBuildable(TilePosition(x, y), true))
				{
					return false;
				}
			}
		}
	}

	// For every tile of an expansion
	for (auto base : TerrainTracker::Instance().getNextExpansion())
	{
		for (int i = 0; i < building.tileWidth(); i++)
		{
			for (int j = 0; j < building.tileHeight(); j++)
			{
				// If the x value of this tile of the building is within an expansion and the y value of this tile of the building is within an expansion, return false
				if (buildTilePosition.x + i >= base.x && buildTilePosition.x + i < base.x + 4 && buildTilePosition.y + j >= base.y && buildTilePosition.y + j < base.y + 3)
				{
					return false;
				}
			}
		}
	}
	// If no issues, return true
	return true;
}

TilePosition BuildingTrackerClass::getBuildLocationNear(UnitType building, TilePosition buildTilePosition, bool ignoreCond)
{
	int x = buildTilePosition.x;
	int y = buildTilePosition.y;
	int length = 1;
	int j = 0;
	bool first = true;
	int dx = 0;
	int dy = 1;

	// Searches in a spiral around the specified tile position
	while (length < 150)
	{
		// If we can build here, return this tile position		
		if (TilePosition(x, y).isValid() && canBuildHere(building, TilePosition(x, y), ignoreCond) == true)
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
	for (TilePosition tile : TerrainTracker::Instance().getActiveExpansion())
	{
		if (building == UnitTypes::Protoss_Assimilator)
		{
			for (auto gas : ResourceTracker::Instance().getMyGas())
			{
				if (gas.second.getType() == UnitTypes::Resource_Vespene_Geyser)
				{
					return gas.second.getTilePosition();
				}
			}
		}
		else if (building == UnitTypes::Protoss_Nexus)
		{
			for (auto base : TerrainTracker::Instance().getNextExpansion())
			{
				if (Broodwar->getUnitsInRadius(Position(base), 128, Filter::IsResourceDepot).size() <= 0)
				{
					return base;
				}
			}
		}
		else
		{
			return getBuildLocationNear(building, tile, false);
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
	// For each building in the protoss race
	for (auto &b : BuildOrderTracker::Instance().getBuildingDesired())
	{
		// If our visible count is lower than our desired count
		if (b.second > Broodwar->self()->visibleUnitCount(b.first) && queuedBuildings.find(b.first) == queuedBuildings.end())
		{
			TilePosition here = BuildingTracker::Instance().getBuildLocation(b.first);
			Unit builder = ProbeTracker::Instance().getClosestProbe(Position(here));			
		
			// If the Tile Position and Builder are valid
			if (here.isValid() && builder)
			{
				// Queue at this building type a pair of building placement and builder
				queuedBuildings.emplace(b.first, make_pair(here, builder));
				GridTracker::Instance().updateReservedLocation(b.first, here);
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

		// If probe died, replace the probe
		if (!b.second.second || !b.second.second->exists())
		{
			b.second.second = ProbeTracker::Instance().getClosestProbe(Position(b.second.first));
			continue;
		}

		// If placement is invalid, replace
		if (!b.second.first.isValid())
		{
			b.second.first = BuildingTracker::Instance().getBuildLocation(b.first);
			continue;
		}		

		// If the Probe is mining a boulder, replace
		if (b.second.second->getTarget() && b.second.second->getTarget()->getType().isMineralField() && b.second.second->getTarget()->getResources() == 0)
		{
			continue;
		}

		// If drawing is on, draw a box around the build position			
		Broodwar->drawLineMap(Position(b.second.first), b.second.second->getPosition(), Broodwar->self()->getColor());
		Broodwar->drawBoxMap(Position(b.second.first), Position(TilePosition(b.second.first.x + b.first.tileWidth(), b.second.first.y + b.first.tileHeight())), Broodwar->self()->getColor());

		// If we issued a command to this Probe already, skip
		if (b.second.second->isConstructing() || b.second.second->getLastCommandFrame() >= Broodwar->getFrameCount())
		{
			continue;
		}

		// If we almost have enough resources, move the Probe to the build position
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
				if (Broodwar->isVisible(TilePosition(x, y)) && (GridTracker::Instance().getReserveGrid(x, y) > 0 || !Broodwar->isBuildable(TilePosition(x, y))))
				{
					b.second.first = BuildingTracker::Instance().getBuildLocation(b.first);
					continue;
				}
			}
		}

		// If Probe is not currently returning minerals or constructing, the build position is valid and can afford the building, then proceed with build
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
	myBuildings[building].setBuildingType(building->getType());
	myBuildings[building].setIdleStatus(building->getRemainingTrainTime() == 0);
	myBuildings[building].setTilePosition(building->getTilePosition());
}

void BuildingTrackerClass::removeBuilding(Unit building)
{
	myBuildings.erase(building);
}