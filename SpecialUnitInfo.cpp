#include "SpecialUnitInfo.h"

SpecialUnitInfoClass::SpecialUnitInfoClass(Position newPosition, Position newDestination)
{
	position = newPosition;
	destination = newDestination;
}

SpecialUnitInfoClass::SpecialUnitInfoClass()
{
	position = Positions::None;
	destination = Positions::None;
}