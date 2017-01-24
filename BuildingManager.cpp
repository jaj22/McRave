#include "BuildingManager.h"
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace std;

void buildingManager(UnitType building, Unit builder)
{
	// For each expansion, check if you can build near it, starting at the main
	for (int i = 0; i < (int)activeExpansion.size() - 1; i++)
	{
		Broodwar << "Attempt at base#: " << i << endl;
		buildTilePosition = getBuildLocationNear(building, builder, activeExpansion.at(i));
		buildPosition = Position(32 * buildTilePosition.x, 32 * buildTilePosition.y);

		// If build position available and not invalid (returns x > 1000)
		if (buildTilePosition != TilePositions::None)
		{
			// If builder is too far away, let's just get a different one, also helps prevent probes from pathing weird and getting stuck
			if (builder->getDistance(Position(32 * buildTilePosition.x, 32 * buildTilePosition.y)) > 2000)
			{
				buildingWorkerID.pop_back();
				buildingWorkerID.push_back((Broodwar->getClosestUnit(buildPosition, Filter::IsAlly && Filter::IsWorker))->getID());
				break;
			}
			// Build
			builder->build(building, buildTilePosition);
			currentBuilding = building;
			break;
		}
	}
}

bool canBuildHere(UnitType building, Unit builder, TilePosition buildTilePosition)
{
	TilePosition expectedRightCorner;

	// Start at one tile vertically above the build site and check the tile width and height + 1 to make sure units can move past and dont get stuck
	for (int x = buildTilePosition.x; x < buildTilePosition.x + building.tileWidth() + 1; x++)
	{
		for (int y = buildTilePosition.y - 1; y < buildTilePosition.y + building.tileHeight() + 1; y++)
		{
			// If the location is outside the boundaries, return false
			if (x <= 0 || x >= Broodwar->mapWidth() - 4 || y <= 0 || y >= Broodwar->mapHeight() - 4)
			{
				return false;
			}
			// If the spot is not buildable, has a building on it or is within 2 tiles of a mineral field, return false
			else if (!builder->canBuild(building, buildTilePosition, true) || Broodwar->isBuildable(TilePosition(x, y), true) == false || Broodwar->getUnitsInRadius(x * 32, y * 32, 62, Filter::IsMineralField).empty() == false)
			{
				return false;
			}
			// If the pylon is within 3 tiles of another pylon, return false
			else if (building == UnitTypes::Protoss_Pylon && Broodwar->getUnitsInRadius(x * 32, y * 32, 128, Filter::GetType == UnitTypes::Protoss_Pylon).size() > 1)
			{
				return false;
			}
		}
	}
	// If the building site has power for buildings that need it (all except nexus/pylon), return true
	if (building == UnitTypes::Protoss_Pylon || Broodwar->hasPower(buildTilePosition, building) == true)
	{
		Broodwar << "Placement is okay" << endl;
		return true;
	}
	// If we missed anything, return false and try again
	Broodwar << "Couldn't place building" << endl;
	return false;
}

TilePosition getBuildLocationNear(UnitType building, Unit builder, TilePosition buildTilePosition)
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
		if (x >= 0 && x < BWAPI::Broodwar->mapWidth() && y >= 0 && y < BWAPI::Broodwar->mapHeight())
		{
			if (canBuildHere(building, builder, TilePosition(x, y)) == true)
			{
				return BWAPI::TilePosition(x, y);
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
	return BWAPI::TilePositions::None;
}

void nexusManager(UnitType building, Unit builder, TilePosition expansion)
{
	TilePosition buildTilePosition;
	buildTilePosition = Broodwar->getBuildLocation(building, expansion);
	builder->build(building, buildTilePosition);
	Broodwar << "Expanding." << std::endl;
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

			if (Broodwar->self()->minerals() >= UpgradeTypes::Singularity_Charge.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Singularity_Charge.gasPrice() + queuedGas)
			{
				building->upgrade(UpgradeTypes::Singularity_Charge);
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
			if (archivesCnt >= 1 && darkTemplarCnt < 10 && Broodwar->self()->minerals() >= UnitTypes::Protoss_Dark_Templar.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Dark_Templar.gasPrice() + queuedGas && Broodwar->self()->supplyUsed() + UnitTypes::Protoss_Dark_Templar.supplyRequired() <= Broodwar->self()->supplyTotal())
			{
				building->train(UnitTypes::Protoss_Dark_Templar);
			}
			else if (coreCnt >= 1 && dragoonCnt < 20 && Broodwar->self()->minerals() >= UnitTypes::Protoss_Dragoon.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Dragoon.gasPrice() + queuedGas && Broodwar->self()->supplyUsed() + UnitTypes::Protoss_Dragoon.supplyRequired() <= Broodwar->self()->supplyTotal())
			{
				building->train(UnitTypes::Protoss_Dragoon);
			}
			else if (zealotCnt < 30 && Broodwar->self()->gas() < UnitTypes::Protoss_Dragoon.gasPrice() || zealotCnt < dragoonCnt && Broodwar->self()->minerals() >= UnitTypes::Protoss_Zealot.mineralPrice() + queuedMineral && Broodwar->self()->supplyUsed() + UnitTypes::Protoss_Zealot.supplyRequired() <= Broodwar->self()->supplyTotal())
			{
				building->train(UnitTypes::Protoss_Zealot);
			}
			break;
		case UnitTypes::Enum::Protoss_Stargate:
			if (fleetBeaconCnt >= 1 && Broodwar->self()->minerals() >= UnitTypes::Protoss_Carrier.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Carrier.gasPrice() + queuedGas)
			{
				building->train(UnitTypes::Protoss_Carrier);
			}
		case UnitTypes::Enum::Protoss_Robotics_Facility:
			if (reaverCnt <= shuttleCnt && Broodwar->self()->minerals() >= UnitTypes::Protoss_Reaver.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Reaver.gasPrice() + queuedGas)
			{
				building->train(UnitTypes::Protoss_Reaver);
			}
			if (reaverCnt > shuttleCnt && Broodwar->self()->minerals() >= UnitTypes::Protoss_Shuttle.mineralPrice() + queuedMineral)
			{
				building->train(UnitTypes::Protoss_Shuttle);
			}
			if (observerCnt < 2 && Broodwar->self()->minerals() >= UnitTypes::Protoss_Observer.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Observer.gasPrice() + queuedGas)
			{
				building->train(UnitTypes::Protoss_Observer);
			}
		}
	}
}