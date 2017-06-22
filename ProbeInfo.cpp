#include "ProbeInfo.h"

ProbeInfo::ProbeInfo()
{
	target = nullptr;
	resource = nullptr;
	storedUnit = nullptr;
	miniTile = WalkPositions::None;
	lastGatherFrame = 0;
	resourcePosition = Positions::None;
}