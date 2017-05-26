#include "UnitInfo.h"

UnitInfoClass::UnitInfoClass()
{
	strength = 0.0;
	maxStrength = 0.0;
	local = 0.0;
	range = 0.0;
	priority = 0.0;
	speed = 0.0;
	deadFrame = 0;
	strategy = 0;
	lastAttackFrame = 0;
	position = Positions::None;
	targetPosition = Positions::None;
	miniTile = WalkPositions::None;
	target = nullptr;
	type = UnitTypes::Enum::None;
	command = UnitCommandTypes::None;	
}
UnitInfoClass::~UnitInfoClass()
{
}