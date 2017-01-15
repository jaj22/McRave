#include "BuildingManager.h"
#include <BWAPI.h>

using namespace BWAPI;


void buildingManager(UnitType building, Unit builder, TilePosition nexus)
{
	TilePosition buildPosition;
	/*buildPosition = Broodwar->getBuildLocation(building, builder->getTilePosition());
	builder->build(building, buildPosition);*/

	buildPosition = getBuildLocationNear(building, builder, nexus);
	if (buildPosition.x < 1000)
	{
		builder->build(building, buildPosition);
	}
}

bool canBuildHere(UnitType building, Unit builder, TilePosition buildPosition)
{
	TilePosition expectedRightCorner;
	expectedRightCorner.x = buildPosition.x + building.tileWidth();
	expectedRightCorner.y = buildPosition.y + building.tileHeight();

	Broodwar->drawBoxMap(buildPosition.x, buildPosition.y, expectedRightCorner.x, expectedRightCorner.y, Colors::Black, true);


	for (int x = buildPosition.x - 1; x < buildPosition.x + building.tileWidth() + 1; x++)
	{
		for (int y = buildPosition.y - 1; y < buildPosition.y + building.tileHeight() + 1; y++)
		{
			if (x <= 5 || x >= Broodwar->mapWidth() || y <= 5 || y >= Broodwar->mapHeight())
			{
				return false;
			}

			else if (Broodwar->isBuildable(TilePosition(x, y), true) == false || Broodwar->getUnitsInRadius(x * 32, y * 32, 200, Filter::IsMineralField).empty() == false)
			{
				return false;
			}
		}
	}
	if (building == UnitTypes::Protoss_Pylon || Broodwar->hasPower(buildPosition, building) == true)
	{
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

TilePosition getBuildLocationNear(UnitType building, Unit builder, TilePosition buildPosition)
{
	//returns a valid build location near the specified tile position.
	//searches outward in a spiral.
	int x = buildPosition.x;
	int y = buildPosition.y;
	int length = 1;
	int j = 0;
	bool first = true;
	int dx = 0;
	int dy = 1;
	while (length < BWAPI::Broodwar->mapWidth()) //We'll ride the spiral to the end
	{
		//if we can build here, return this tile position
		if (x >= 0 && x < BWAPI::Broodwar->mapWidth() && y >= 0 && y < BWAPI::Broodwar->mapHeight())
		{
			if (canBuildHere(building, builder, TilePosition(x, y)) == true)
			{
				return BWAPI::TilePosition(x, y);
			}
		}
		//otherwise, move to another position
		x = x + dx;
		y = y + dy;
		//count how many steps we take in this direction
		j++;
		if (j == length) //if we've reached the end, its time to turn
		{
			//reset step counter
			j = 0;

			//Spiral out. Keep going.
			if (!first)
				length++; //increment step counter if needed

			//first=true for every other turn so we spiral out at the right rate
			first = !first;

			//turn counter clockwise 90 degrees:
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

			//Spiral out. Keep going.
		}
	}
	return BWAPI::TilePositions::None;
}

void nexusManager(UnitType building, Unit builder, TilePosition expansion)
{
	TilePosition buildPosition;
	buildPosition = Broodwar->getBuildLocation(building, expansion);
	builder->build(building, buildPosition);
	Broodwar << "Expanding." << builder->getID() << std::endl;
}