#include "BuildingManager.h"
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace std;
using namespace BWTA;

// Variables for BuildingManager.cpp
TilePosition buildTilePosition;

TilePosition nexusManager()
{
	for (auto base : nextExpansion)
	{
		if (Broodwar->getUnitsInRadius(Position(base), 128, Filter::IsResourceDepot).size() <= 0)
		{
			return base;
		}
	}
	return TilePositions::None;
}

TilePosition cannonManager(TilePosition base)
{
	// Get angle of mineral line	
	int avgX = 0, avgY = 0, size = 0;
	for (auto m : Broodwar->getUnitsInRadius(Position(base), 320, Filter::IsMineralField))
	{
		avgX = avgX + m->getTilePosition().x;
		avgY = avgY + m->getTilePosition().y;
		size++;
	}
	if (size == 0)
	{
		return TilePositions::None;
	}

	avgX = avgX / size;
	avgY = avgY / size;

	Broodwar->drawCircleMap(Position(avgX * 32, avgY * 32), 10, Colors::Red, true);
	return getBuildLocationNear(UnitTypes::Protoss_Photon_Cannon, TilePosition(avgX, base.y + (2 * (base.y - avgY))));
}

TilePosition buildingManager(UnitType building)
{
	if (building == UnitTypes::Protoss_Nexus)
	{
		return nexusManager();
	}

	// For each expansion, check if you can build near it, starting at the main
	for (TilePosition tile : activeExpansion)
	{
		if (building == UnitTypes::Protoss_Shield_Battery)
		{
			buildTilePosition = getBuildLocationNear(building, TilePosition((getNearestChokepoint(nextExpansion.at(0))->getCenter())));
		}
		else if (building == UnitTypes::Protoss_Assimilator)
		{
			for (auto gas : geysers)
			{
				if (Broodwar->canBuildHere(gas->getTilePosition(), UnitTypes::Protoss_Assimilator))
				{
					return gas->getTilePosition();
				}
			}
		}
		else
		{
			buildTilePosition = getBuildLocationNear(building, tile);
		}
		// If build position available and not invalid (returns x > 1000)
		if (buildTilePosition != TilePositions::None && buildTilePosition != TilePositions::Invalid)
		{
			return buildTilePosition;
		}
	}
	return TilePositions::None;
}

bool canBuildHere(UnitType building, TilePosition buildTilePosition)
{
	// TEMPEORARY CHANGES:
	// +1 and -1 on end/start
	// mod 2 x mod 3 y
	// Start at one tile vertically above the build site and check the tile width and height + 1 to make sure units can move past and dont get stuck

	if (building == UnitTypes::Protoss_Photon_Cannon)
	{
		if (Broodwar->canBuildHere(buildTilePosition, building))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if (buildTilePosition.x % 2 == 0 || buildTilePosition.x % 3 == 0 || buildTilePosition.y % 2 == 0)
	{
		return false;
	}


	for (int x = buildTilePosition.x; x <= buildTilePosition.x + building.tileWidth(); x++)
	{
		for (int y = buildTilePosition.y; y <= buildTilePosition.y + building.tileHeight(); y++)
		{
			// If the location is outside the boundaries, return false
			if (x < 0 || x > Broodwar->mapWidth() || y < 0 || y > Broodwar->mapHeight())
			{
				return false;
			}

			// If the spot is not buildable, has a building on it or is within 2 tiles of a mineral field, return false
			else if (Broodwar->isBuildable(TilePosition(x, y), true) == false || (mineralHeatmap[x][y] > 0 && building != UnitTypes::Protoss_Photon_Cannon))
			{
				return false;
			}

			else if (building == UnitTypes::Protoss_Photon_Cannon && Broodwar->getUnitsInRadius(x * 32, y * 32, 32, Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Probe).size() > 0)
			{
				return false;
			}

			// TESTING -- Pylon spreading
			else if (building == UnitTypes::Protoss_Pylon && Broodwar->getUnitsInRadius(x * 32, y * 32, 128, Filter::GetType == UnitTypes::Protoss_Pylon).size() > 1)
			{
				return false;
			}
		}
	}
	// If building is on an expansion tile, don't build there	
	for (auto base : nextExpansion)
	{
		for (int i = 0; i <= building.tileWidth() + 1; i++)
		{
			for (int j = 0; j <= building.tileHeight() + 1; j++)
			{
				// If the x value of this tile of the building is within an expansion, return false
				if (buildTilePosition.x - i >= base.x && buildTilePosition.x - i <= base.x && buildTilePosition.y - j >= base.y && buildTilePosition.y - j <= base.y)
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
	// If we missed anything, return false and try again	
	return false;
}

TilePosition getBuildLocationNear(UnitType building, TilePosition buildTilePosition)
{
	int x = buildTilePosition.x;
	int y = buildTilePosition.y;
	int length = 1;
	int j = 0;
	bool first = true;
	int dx = 0;
	int dy = 1;
	// Searches in a spiral around the specified tile position (usually a nexus)
	while (length < 50)
	{
		//If we can build here, return this tile position
		if (x > 0 && x < Broodwar->mapWidth() && y > 0 && y < Broodwar->mapHeight())
		{
			if (canBuildHere(building, TilePosition(x, y)) == true)
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

void productionManager(Unit building)
{
	if (building->isIdle())
	{
		switch (building->getType())
		{
			// Research Buildings
		case UnitTypes::Enum::Protoss_Forge:
			if (Broodwar->self()->minerals() >= UpgradeTypes::Protoss_Ground_Weapons.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Protoss_Ground_Weapons.gasPrice() + queuedGas)
			{
				building->upgrade(UpgradeTypes::Protoss_Ground_Weapons);
			}
			if (Broodwar->self()->minerals() >= UpgradeTypes::Protoss_Plasma_Shields.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Protoss_Plasma_Shields.gasPrice() + queuedGas)
			{
				building->upgrade(UpgradeTypes::Protoss_Plasma_Shields);
			}
			if (Broodwar->self()->minerals() >= UpgradeTypes::Protoss_Ground_Armor.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Protoss_Ground_Armor.gasPrice() + queuedGas)
			{
				building->upgrade(UpgradeTypes::Protoss_Ground_Armor);
			}
			break;
		case UnitTypes::Enum::Protoss_Cybernetics_Core:

			// If we need Dragoon range upgrade
			if (!Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge) && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon) >= 1)
			{
				if (Broodwar->self()->minerals() >= UpgradeTypes::Singularity_Charge.mineralPrice() && Broodwar->self()->gas() >= UpgradeTypes::Singularity_Charge.gasPrice())
				{
					building->upgrade(UpgradeTypes::Singularity_Charge);
					idleUpgrade.erase(building->getID());
				}
				else
				{
					idleUpgrade.emplace(building->getID(), UpgradeTypes::Singularity_Charge);
				}
			}
			break;
		case UnitTypes::Enum::Protoss_Robotics_Support_Bay:

			if (Broodwar->self()->minerals() >= UpgradeTypes::Scarab_Damage.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Scarab_Damage.gasPrice() + queuedGas)
			{
				building->upgrade(UpgradeTypes::Scarab_Damage);
			}
			if (Broodwar->self()->minerals() >= UpgradeTypes::Reaver_Capacity.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Reaver_Capacity.gasPrice() + queuedGas)
			{
				building->upgrade(UpgradeTypes::Reaver_Capacity);
			}
			/*
			if (Broodwar->self()->minerals() >= UpgradeTypes::Gravitic_Boosters.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Gravitic_Boosters.gasPrice() + queuedGas)
			{
			building->upgrade(UpgradeTypes::Gravitic_Boosters);
			}*/
			break;
		case UnitTypes::Enum::Protoss_Fleet_Beacon:
			if (Broodwar->self()->minerals() >= UpgradeTypes::Carrier_Capacity.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Carrier_Capacity.gasPrice() + queuedGas)
			{
				building->upgrade(UpgradeTypes::Carrier_Capacity);
			}
			break;
		case UnitTypes::Enum::Protoss_Citadel_of_Adun:
			if (Broodwar->self()->minerals() >= UpgradeTypes::Leg_Enhancements.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Leg_Enhancements.gasPrice() + queuedGas)
			{
				building->upgrade(UpgradeTypes::Leg_Enhancements);
			}
			break;

			// Production Buildings
		case UnitTypes::Enum::Protoss_Gateway:
			// If we need a High Templar
			if (Broodwar->self()->hasResearched(TechTypes::Psionic_Storm) && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Templar_Archives) >= 1 && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar) < 5)
			{
				// If we can afford a High Temlar, train
				if (Broodwar->self()->minerals() >= UnitTypes::Protoss_High_Templar.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_High_Templar.gasPrice() + queuedGas + reservedGas && Broodwar->self()->supplyUsed() + UnitTypes::Protoss_High_Templar.supplyRequired() <= Broodwar->self()->supplyTotal())
				{
					building->train(UnitTypes::Protoss_High_Templar);
					idleGates.erase(building->getID());
					return;
				}
				else
				{
					idleGates.emplace(building->getID(), UnitTypes::Protoss_High_Templar);
				}
			}
			// If we need a Dragoon
			if (unitScore[UnitTypes::Protoss_Dragoon] >= unitScore[UnitTypes::Protoss_Zealot] && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) >= 1)
			{
				// If we can afford a Dragoon, train
				if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Dragoon.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Dragoon.gasPrice() + queuedGas + reservedGas && Broodwar->self()->supplyUsed() + UnitTypes::Protoss_Dragoon.supplyRequired() <= Broodwar->self()->supplyTotal())
				{
					building->train(UnitTypes::Protoss_Dragoon);
					idleGates.erase(building->getID());
					return;
				}
				else
				{
					idleGates.emplace(building->getID(), UnitTypes::Protoss_Dragoon);
				}
			}
			// If we need a Zealot
			if (unitScore[UnitTypes::Protoss_Dragoon] < unitScore[UnitTypes::Protoss_Zealot] || Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) < 1 || Broodwar->self()->gas() < UnitTypes::Protoss_Dragoon.gasPrice() + queuedGas + reservedGas)
			{
				// If we can afford a Zealot, train
				if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Zealot.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->supplyUsed() + UnitTypes::Protoss_Zealot.supplyRequired() <= Broodwar->self()->supplyTotal())
				{
					building->train(UnitTypes::Protoss_Zealot);
					idleGates.erase(building->getID());
					return;
				}
				else
				{
					idleGates.emplace(building->getID(), UnitTypes::Protoss_Zealot);
				}
			}
			break;
		case UnitTypes::Enum::Protoss_Stargate:
			/*if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Fleet_Beacon) >= 1 && Broodwar->self()->minerals() >= UnitTypes::Protoss_Carrier.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Carrier.gasPrice() + queuedGas)
			{
			building->train(UnitTypes::Protoss_Carrier);
			}*/
			if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter_Tribunal) >= 1 && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter) < 3)
			{
				if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Arbiter.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Arbiter.gasPrice() + queuedGas)
				{
					building->train(UnitTypes::Protoss_Arbiter);
					idleBuildings.erase(building->getID());
				}
				else
				{
					idleBuildings.emplace(building->getID(), UnitTypes::Protoss_Arbiter);
				}
			}
			// Only build corsairs against Zerg
			if (Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Corsair) < 10)
			{
				if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Corsair.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Corsair.gasPrice() + queuedGas)
				{
					building->train(UnitTypes::Protoss_Corsair);
				}
			}
			break;
		case UnitTypes::Enum::Protoss_Robotics_Facility:
			// If we need an Observer
			if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Observer) < (floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver) / 3) + 1))
			{
				// If we can afford an Observer, train, otherwise, add to priority
				if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Observer.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Observer.gasPrice() + queuedGas)
				{
					building->train(UnitTypes::Protoss_Observer);
					idleBuildings.erase(building->getID());
				}
				else
				{
					idleBuildings.emplace(building->getID(), UnitTypes::Protoss_Observer);
				}
			}
			// If we need a Reaver
			//else if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver) <= Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Shuttle) || Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Shuttle) >= 3)
			else if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver) < 10 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Support_Bay) > 0)
			{
				// If we can afford a Reaver, train, otherwise, add to priority
				if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Reaver.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Reaver.gasPrice() + queuedGas)
				{
					building->train(UnitTypes::Protoss_Reaver);
					idleBuildings.erase(building->getID());
				}
				else
				{
					idleBuildings.emplace(building->getID(), UnitTypes::Protoss_Reaver);
				}
			}
			// If we need a Shuttle
			//else if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver) > Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Shuttle) || Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Shuttle) < 3)
			//{
			//	// If we can afford a Shuttle, train, otherwise, add to priority
			//	if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Shuttle.mineralPrice() + queuedMineral)
			//	{
			//		building->train(UnitTypes::Protoss_Shuttle);
			//		idleBuildings.erase(building->getID());
			//	}
			//	else
			//	{
			//		idleBuildings.emplace(building->getID(), UnitTypes::Protoss_Shuttle);
			//	}
			//}
			break;
			// Tech Research
		case UnitTypes::Enum::Protoss_Templar_Archives:
			if (!Broodwar->self()->hasResearched(TechTypes::Psionic_Storm))
			{
				if (Broodwar->self()->minerals() >= TechTypes::Psionic_Storm.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= TechTypes::Psionic_Storm.gasPrice() + queuedGas)
				{
					building->research(TechTypes::Psionic_Storm);
					idleTech.erase(building->getID());
				}
				else
				{
					idleTech.emplace(building->getID(), TechTypes::Psionic_Storm);
				}
			}
			break;
		case UnitTypes::Enum::Protoss_Arbiter_Tribunal:
			if (!Broodwar->self()->hasResearched(TechTypes::Stasis_Field))
			{
				if (Broodwar->self()->minerals() >= TechTypes::Stasis_Field.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= TechTypes::Stasis_Field.gasPrice() + queuedGas)
				{
					building->research(TechTypes::Stasis_Field);
					idleTech.erase(building->getID());
				}
				else
				{
					idleTech.emplace(building->getID(), TechTypes::Stasis_Field);
				}
			}
			break;
		}
	}
}