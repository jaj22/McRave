#include "BuildingManager.h"
#include "GridManager.h"
#include "TerrainManager.h"
#include "ResourceManager.h"
#include "BuildOrder.h"
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace std;
using namespace BWTA;

TilePosition buildTilePosition;

bool canBuildHere(UnitType building, TilePosition buildTilePosition, bool ignoreCond)
{
	// Offset for first pylon
	int offset = 0;
	if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Pylon) == 0)
	{
		offset = 2;
	}

	// Space out by at least 1 tile every 4 tiles horizontally, 3 vertically
	if (!ignoreCond && (buildTilePosition.x % 3 == 0 || buildTilePosition.x % 2 == 0 || buildTilePosition.y % 3 == 0 || Broodwar->canBuildHere(buildTilePosition, building, nullptr, true) == false))
	{
		return false;
	}

	// For every tile of a buildings size
	for (int x = buildTilePosition.x - offset; x <= buildTilePosition.x + building.tileWidth() + offset; x++)
	{
		for (int y = buildTilePosition.y - offset; y <= buildTilePosition.y + building.tileHeight() + offset; y++)
		{
			// If the location is outside the boundaries of the map, return false
			if (x < 0 || x > Broodwar->mapWidth() || y < 0 || y > Broodwar->mapHeight())
			{
				return false;
			}
			// If the spot is not buildable, has a building on it or is within 2 tiles of a mineral field, return false
			if ((building != UnitTypes::Protoss_Photon_Cannon && GridTracker::Instance().getResourceGrid(x, y) > 0) || (!ignoreCond && building == UnitTypes::Protoss_Pylon && Broodwar->getUnitsInRadius(x * 32, y * 32, 128, Filter::GetType == UnitTypes::Protoss_Pylon).size() > 0) || TerrainTracker::Instance().getAllyTerritory().find(getRegion(buildTilePosition)) == TerrainTracker::Instance().getAllyTerritory().end())
			{
				return false;
			}
			if (getNearestChokepoint(TilePosition(x, y)) && getNearestChokepoint(TilePosition(x, y))->getCenter().getDistance(Position(TilePosition(x, y))) < 16)
			{
				return false;
			}
			/*if (!ignoreCond && GridTracker::Instance().getMobilityGrid(x, y) <= 9)
			{
				return false;
			}*/
		}
	}
	// For every tile of an expansion
	for (auto base : TerrainTracker::Instance().getNextExpansion())
	{
		for (int i = 0; i <= building.tileWidth() + 1; i++)
		{
			for (int j = 0; j <= building.tileHeight() + 1; j++)
			{
				// If the x value of this tile of the building is within an expansion and the y value of this tile of the building is within an expansion, return false
				if (buildTilePosition.x + i >= base.x && buildTilePosition.x + i <= base.x + 4 && buildTilePosition.y + j >= base.y && buildTilePosition.y + j <= base.y + 3)
				{
					return false;
				}
			}
		}
	}
	// If the building site has power for buildings that need it (all except nexus/pylon), return true
	if (building == UnitTypes::Protoss_Pylon || Broodwar->hasPower(buildTilePosition, building) == true)
	{
		return true;
	}
	// If we missed anything, return false
	return false;
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
	// Searches in a spiral around the specified tile position (usually a nexus)
	while (length < 80)
	{
		//If we can build here, return this tile position
		if (x > 0 && x < Broodwar->mapWidth() && y > 0 && y < Broodwar->mapHeight())
		{
			if (canBuildHere(building, TilePosition(x, y), ignoreCond) == true)
			{
				return TilePosition(x, y);
			}
		}
		//Otherwise, move to another position
		x = x + dx;
		y = y + dy;
		//Count how many steps we take in this direction
		j++;
		if (j == length) //if we've reached the end, its time to turn
		{
			//reset step counter
			j = 0;

			//Increment step counter
			if (!first)
				length++;

			//First=true for every other turn so we spiral out at the right rate
			first = !first;

			//Turn counter clockwise 90 degrees:
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
	return TilePositions::None;
}

TilePosition BuildingTrackerClass::getBuildLocation(UnitType building)
{
	// For each expansion, check if you can build near it, starting at the main
	for (TilePosition tile : TerrainTracker::Instance().getActiveExpansion())
	{
		if (building == UnitTypes::Protoss_Assimilator)
		{
			buildTilePosition = getGasLocation();
		}
		else if (building == UnitTypes::Protoss_Nexus)
		{
			buildTilePosition = getNexusLocation();
		}
		else
		{
			buildTilePosition = getBuildLocationNear(building, tile, false);
		}
		// If build position available and not invalid (returns x > 1000)
		if (buildTilePosition != TilePositions::None && buildTilePosition != TilePositions::Invalid)
		{
			return buildTilePosition;
		}
	}
	return TilePositions::None;
}

TilePosition BuildingTrackerClass::getGasLocation()
{
	for (auto gas : ResourceTracker::Instance().getMyGas())
	{
		if (gas.second.getUnitType() == UnitTypes::Resource_Vespene_Geyser)
		{
			return gas.second.getTilePosition();
		}
	}
	return TilePositions::None;
}

TilePosition BuildingTrackerClass::getNexusLocation()
{
	for (auto base : TerrainTracker::Instance().getNextExpansion())
	{
		if (Broodwar->getUnitsInRadius(Position(base), 128, Filter::IsResourceDepot).size() <= 0)
		{
			return base;
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
			Unit builder = Broodwar->getClosestUnit(Position(here), Filter::IsAlly && Filter::IsWorker && !Filter::IsCarryingSomething && !Filter::IsGatheringGas);
			// If the Tile Position and Builder are valid
			if (here != TilePositions::None && builder)
			{
				// Queue at this building type a pair of building placement and builder
				queuedBuildings.emplace(b.first, make_pair(here, builder));
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
			b.second.second = Broodwar->getClosestUnit(Position(b.second.first), Filter::IsAlly && Filter::IsWorker && !Filter::IsCarryingSomething && !Filter::IsGatheringGas);
			continue;
		}

		// If can't build here right now and the tile is visible, replace the building position
		if (!Broodwar->canBuildHere(b.second.first, b.first, b.second.second) && Broodwar->isVisible(b.second.first))
		{
			b.second.first = BuildingTracker::Instance().getBuildLocation(b.first);
			continue;
		}

		// If the Probe has a target
		if (b.second.second->getTarget())
		{
			// If the target has a resource count of 0 (mineral blocking a ramp), let Probe continue mining it
			if (b.second.second->getTarget()->getResources() == 0)
			{
				continue;
			}
		}

		// If drawing is on, draw a box around the build position			
		Broodwar->drawLineMap(Position(b.second.first), b.second.second->getPosition(), Broodwar->self()->getColor());
		Broodwar->drawBoxMap(Position(b.second.first), Position(TilePosition(b.second.first.x + b.first.tileWidth(), b.second.first.y + b.first.tileHeight())), Broodwar->self()->getColor());


		// If we issued a command to this Probe already, skip
		if (b.second.second->isConstructing() || b.second.second->getLastCommandFrame() >= Broodwar->getFrameCount() && (b.second.second->getLastCommand().getType() == UnitCommandTypes::Move || b.second.second->getLastCommand().getType() == UnitCommandTypes::Build))
		{
			continue;
		}

		// If we almost have enough resources, move the Probe to the build position
		if (Broodwar->self()->minerals() >= 0.8*b.first.mineralPrice() && Broodwar->self()->minerals() <= b.first.mineralPrice() && Broodwar->self()->gas() >= 0.8*b.first.gasPrice() && Broodwar->self()->gas() <= b.first.gasPrice() || (b.second.second->getDistance(Position(b.second.first)) > 160 && Broodwar->self()->minerals() >= 0.8*b.first.mineralPrice() && 0.8*Broodwar->self()->gas() >= b.first.gasPrice()))
		{
			b.second.second->move(Position(b.second.first));
			continue;
		}

		// If Probe is not currently returning minerals or constructing, the build position is valid and can afford the building, then proceed with build
		else if (b.second.first != TilePositions::None && Broodwar->self()->minerals() >= b.first.mineralPrice() && Broodwar->self()->gas() >= b.first.gasPrice())
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