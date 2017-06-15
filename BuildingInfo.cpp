#include "BuildingInfo.h"

BuildingInfo::BuildingInfo()
{
	idleStatus = true;
	buildingType = UnitTypes::None;
	nextUnit = UnitTypes::None;
	buildingTilePosition = TilePositions::None;
}

BuildingInfo::~BuildingInfo()
{
}