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

// Accessors
int ResourceInfo::getGathererCount() const
{
	return gathererCount;
}
int ResourceInfo::getRemainingResources() const
{
	return remainingResources;
}
Unit ResourceInfo::getClosestNexus() const
{
	return nexus;
}
Position ResourceInfo::getPosition() const
{
	return resourcePosition;
}
TilePosition ResourceInfo::getTilePosition() const
{
	return resourceTilePosition;
}
UnitType ResourceInfo::getUnitType() const
{
	return unitType;
}

// Mutators
void ResourceInfo::setGathererCount(int newGathererCount)
{
	gathererCount = newGathererCount;
}
void ResourceInfo::setRemainingResources(int newRemainingResources)
{
	remainingResources = newRemainingResources;
}
void ResourceInfo::setClosestNexus(Unit newNexus)
{
	nexus = newNexus;
}
void ResourceInfo::setPosition(Position newResourcePosition)
{
	resourcePosition = newResourcePosition;
}
void ResourceInfo::setTilePosition(TilePosition newResourceTilePosition)
{
	resourceTilePosition = newResourceTilePosition;
}
void ResourceInfo::setUnitType(UnitType newUnitType)
{
	unitType = newUnitType;
}