#include "SpecialUnitInfo.h"

SpecialUnitInfoClass::SpecialUnitInfoClass()
{
	position = Positions::None;
	destination = Positions::None;
	miniTile = WalkPositions::None;
	shuttlePair = nullptr;
}

SpecialUnitInfoClass::~SpecialUnitInfoClass()
{
}