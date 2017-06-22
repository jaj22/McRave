#include "WorkerInfo.h"

WorkerInfo::WorkerInfo()
{
	thisUnit = nullptr;
	target = nullptr;
	resource = nullptr;

	position = Positions::None;
	walkPosition = WalkPositions::None;
	tilePosition = TilePositions::None;
	lastGatherFrame = 0;
}