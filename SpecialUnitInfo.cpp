#include "SpecialUnitInfo.h"

SupportUnitInfo::SupportUnitInfo()
{
	position = Positions::None;
	destination = Positions::None;
	miniTile = WalkPositions::None;
}

SupportUnitInfo::~SupportUnitInfo()
{
}

TransportInfo::TransportInfo()
{
	position = Positions::None;
	destination = Positions::None;
	drop = Positions::None;
	miniTile = WalkPositions::None;
	transport = nullptr;
	target = nullptr;
	loadState = 0;
	cargoSize = 0;
	harassing = false;
}

TransportInfo::~TransportInfo()
{

}

void TransportInfo::assignCargo(Unit unit)
{
	assignedCargo.emplace(unit);
	cargoSize = cargoSize + unit->getType().spaceRequired();
}

void TransportInfo::removeCargo(Unit unit)
{
	assignedCargo.erase(unit);
	cargoSize = cargoSize - unit->getType().spaceRequired();
}