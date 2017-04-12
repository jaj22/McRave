#include "ResourceManager.h"

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
ResourceInfo::ResourceInfo(int newGathererCount, int newRemainingResources, Position newPosition)
{
	gathererCount = newGathererCount;
	remainingResources = newRemainingResources;
	resourcePosition = newPosition;
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
Position ResourceInfo::getPosition() const
{
	return resourcePosition;
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
void ResourceInfo::setPosition(Position newResourcePosition)
{
	resourcePosition = newResourcePosition;
}

// Resource updating

void updateResources(Unit resource, map <Unit, ResourceInfo>& myResources)
{
	// Each resource has a probe count, can update when probe assigned/unassigned (death or otherwise)
	// Position should always be constant
	// Remaining resources can be updated every frame along with resource iterator (can monitor if expansions are needed easier)
}