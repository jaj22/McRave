#include "WorkerInfo.h"

WorkerInfo::WorkerInfo()
{
	thisUnit = nullptr;
	target = nullptr;
	resource = nullptr;

	unitType = UnitTypes::None;
	buildingType = UnitTypes::None;

	position = Positions::None;
	walkPosition = WalkPositions::None;
	tilePosition = TilePositions::None;
	buildPosition = TilePositions::None;
	lastGatherFrame = 0;
}