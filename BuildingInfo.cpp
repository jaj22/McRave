#include "BuildingInfo.h"

BuildingInfo::BuildingInfo()
{
	idleStatus = true;
	storedUnit = nullptr;
	buildingType = UnitTypes::None;
	nextUnit = UnitTypes::None;
	buildingTilePosition = TilePositions::None;
}

BuildingInfo::~BuildingInfo()
{
}