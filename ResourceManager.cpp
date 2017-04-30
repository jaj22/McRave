#include "ResourceManager.h"

using namespace BWAPI;
using namespace std;

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

// Resource updating
void storeMineral(Unit resource, map <Unit, ResourceInfo>& myMinerals)
{
	// If this is a new unit, initialize at 0 workers, initial resources and find position
	ResourceInfo newResource(0, resource->getInitialResources(), resource->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Nexus), resource->getPosition(), resource->getTilePosition(), resource->getType());
	myMinerals[resource] = newResource;
	return;
}
void storeGas(Unit resource, map <Unit, ResourceInfo>& myGas)
{
	// If this is a new unit, initialize at 0 workers, initial resources and find position
	ResourceInfo newResource(0, resource->getInitialResources(), resource->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Nexus), resource->getPosition(), resource->getTilePosition(), resource->getType());
	myGas[resource] = newResource;
	return;
}
Unit assignResource(map <Unit, ResourceInfo>& myMinerals, map <Unit, ResourceInfo>& myGas)
{
	int cnt = 1;
	for (auto &gas : myGas)
	{
		if (gas.second.getUnitType() == UnitTypes::Protoss_Assimilator && gas.first->isCompleted() && gas.second.getGathererCount() < 3)
		{
			gas.second.setGathererCount(gas.second.getGathererCount() + 1);
			return gas.first;
		}
	}

	// First checks if a mineral field has 0 Probes mining, if none, checks if a mineral field has 1 Probe mining. Assigns to 0 first, then 1. Spreads saturation.
	while (cnt <= 2)
	{
		for (auto &mineral : myMinerals)
		{
			if (mineral.second.getGathererCount() < cnt)
			{
				mineral.second.setGathererCount(mineral.second.getGathererCount() + 1);
				return mineral.first;
			}
		}
		cnt++;
	}
	return nullptr;
}