#include "BuildingInfo.h"

BuildingInfoClass::BuildingInfoClass()
{
	idleStatus = true;
	buildingType = UnitTypes::None;
	nextUnit = UnitTypes::None;
	buildingTilePosition = TilePositions::None;
}

BuildingInfoClass::~BuildingInfoClass()
{
}