#include "BaseInfo.h"

BaseInfo::BaseInfo()
{
	defenseCount = 0;
	thisUnit = nullptr;
	unitType = UnitTypes::None;
	position = Positions::None;
	defensePosition = TilePositions::None;
	tilePosition = TilePositions::None;
	region = nullptr;
}