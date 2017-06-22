#include "UnitInfo.h"

UnitInfo::UnitInfo()
{
	strength = 0.0;
	maxStrength = 0.0;
	local = 0.0;
	groundRange = 0.0;
	airRange = 0.0;
	priority = 0.0;
	groundDamage = 0.0;
	airDamage = 0.0;
	speed = 0.0;
	deadFrame = 0;
	strategy = 0;
	lastAttackFrame = 0;
	position = Positions::None;
	targetPosition = Positions::None;
	walkPosition = WalkPositions::None;
	target = nullptr;
	unitType = UnitTypes::Enum::None;
	command = UnitCommandTypes::None;	
	transport = false;
}
UnitInfo::~UnitInfo()
{
}