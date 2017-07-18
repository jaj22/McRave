#include "ResourceInfo.h"

ResourceInfo::ResourceInfo()
{	
	gathererCount = 0;
	remainingResources = 0;
	storedUnit = nullptr;
	unitType = UnitTypes::None;
	position = Positions::None;
	resourceClusterPosition = Positions::None;
	baseCenter = Positions::None;
	tilePosition = TilePositions::None;
	walkPosition = WalkPositions::None;
}
ResourceInfo::~ResourceInfo()
{

}
