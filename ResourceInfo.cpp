#include "ResourceInfo.h"

// Constructors
ResourceInfo::ResourceInfo()
{
	gathererCount = 0;
	remainingResources = 0;
	resourcePosition = Positions::None;
}
ResourceInfo::~ResourceInfo()
{

}
ResourceInfo::ResourceInfo(int newGathererCount, int newRemainingResources, Unit newNexus, Position newPosition, TilePosition newTilePosition, UnitType newUnitType)
{
	gathererCount = newGathererCount;
	remainingResources = newRemainingResources;
	nexus = newNexus;
	resourcePosition = newPosition;
	resourceTilePosition = newTilePosition;
	unitType = newUnitType;
}