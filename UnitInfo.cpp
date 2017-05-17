#include "UnitInfo.h"

UnitInfoClass::UnitInfoClass(UnitType newType, Position newPosition, double newStrength, double newMaxStrength, double newRange, double newPriority, UnitCommandType newCommand, int newDeadFrame, int newStrategy, int newCommandFrame, Unit newTarget, WalkPosition newMiniTile)
{
	unitType = newType;
	unitPosition = newPosition;
	unitStrength = newStrength;
	unitMaxStrength = newMaxStrength;
	unitRange = newRange;
	unitPriority = newPriority;
	unitCommand = newCommand;
	deadFrame = newDeadFrame;
	strategy = newStrategy;
	lastCommandFrame = newCommandFrame;
	target = newTarget;
	miniTile = newMiniTile;
}
UnitInfoClass::UnitInfoClass()
{
	unitType = UnitTypes::Enum::None;
	unitPosition = Positions::None;
	unitStrength = 0.0;
	unitMaxStrength = 0.0;
	unitRange = 0;
	unitCommand = UnitCommandTypes::None;
	deadFrame = 0;
	strategy = 0;
	lastCommandFrame = 0;
	target = nullptr;
	miniTile = WalkPositions::None;
}
UnitInfoClass::~UnitInfoClass()
{
}

Position UnitInfoClass::getPosition() const
{
	return unitPosition;
}
Position UnitInfoClass::getTargetPosition() const
{
	return targetPosition;
}
UnitType UnitInfoClass::getUnitType() const
{
	return unitType;
}
double UnitInfoClass::getStrength() const
{
	return unitStrength;
}
double UnitInfoClass::getMaxStrength() const
{
	return unitMaxStrength;
}
double UnitInfoClass::getRange() const
{
	return unitRange;
}
double UnitInfoClass::getLocal() const
{
	return unitLocal;
}
double UnitInfoClass::getPriority() const
{
	return unitPriority;
}
UnitCommandType UnitInfoClass::getCommand() const
{
	return unitCommand;
}
Unit UnitInfoClass::getTarget() const
{
	return target;
}
int UnitInfoClass::getDeadFrame() const
{
	return deadFrame;
}
int UnitInfoClass::getStrategy() const
{
	return strategy;
}
int UnitInfoClass::getLastCommandFrame() const
{
	return lastCommandFrame;
}
WalkPosition UnitInfoClass::getMiniTile() const
{
	return miniTile;
}

void UnitInfoClass::setUnitType(UnitType newUnitType)
{
	unitType = newUnitType;
}
void UnitInfoClass::setPosition(Position newPosition)
{
	unitPosition = newPosition;
}
void UnitInfoClass::setTargetPosition(Position newTargetPosition)
{
	targetPosition = newTargetPosition;
}
void UnitInfoClass::setStrength(double newStrength)
{
	unitStrength = newStrength;
}
void UnitInfoClass::setMaxStrength(double newMaxStrength)
{
	unitMaxStrength = newMaxStrength;
}
void UnitInfoClass::setLocal(double newUnitLocal)
{
	unitLocal = newUnitLocal;
}
void UnitInfoClass::setRange(double newRange)
{
	unitRange = newRange;
}
void UnitInfoClass::setPriority(double newPriority)
{
	unitPriority = newPriority;
}
void UnitInfoClass::setCommand(UnitCommandType newCommand)
{
	unitCommand = newCommand;
}
void UnitInfoClass::setTarget(Unit newTarget)
{
	target = newTarget;
}
void UnitInfoClass::setDeadFrame(int newDeadFrame)
{
	deadFrame = newDeadFrame;
}
void UnitInfoClass::setStrategy(int newStrategy)
{
	strategy = newStrategy;
}
void UnitInfoClass::setLastCommandFrame(int newCommandFrame)
{
	lastCommandFrame = newCommandFrame;
}
void UnitInfoClass::setMiniTile(WalkPosition newMiniTile)
{
	miniTile = newMiniTile;
}
