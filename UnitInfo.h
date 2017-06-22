#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

// Class for storing information about all units
class UnitInfo {
	double strength, maxStrength, local, groundRange, airRange, priority, groundDamage, airDamage, speed;
	int deadFrame, strategy, lastAttackFrame, currentGoal;	

	Unit target, thisUnit, transport;
	UnitType unitType;
	UnitCommandType command;

	Position position, targetPosition;
	WalkPosition walkPosition, targetWalkPosition;
	TilePosition tilePosition, targetTilePosition;
public:
	UnitInfo();
	~UnitInfo();

	double getStrength() { return strength; }
	double getMaxStrength() { return maxStrength; }
	double getLocal() { return local; }
	double getGroundRange() { return groundRange; }
	double getAirRange() { return airRange; }
	double getPriority() { return priority; }
	double getGroundDamage(){ return groundDamage; }
	double getAirDamage() { return airDamage; }
	double getSpeed() { return speed; }
	int getDeadFrame() { return deadFrame; }
	int getStrategy() { return strategy; }
	int getLastAttackFrame() { return lastAttackFrame; }
	int getCurrentGoal() { return currentGoal; }

	Unit unit() { return thisUnit; }
	Unit getTarget() { return target; }
	Unit getTransport() { return transport; }	
	UnitType getType(){ return unitType; }
	UnitCommandType getCommand() { return command; }

	Position getPosition(){ return position; }
	Position getTargetPosition() { return targetPosition; }	
	WalkPosition getWalkPosition() { return walkPosition; }
	WalkPosition getTargetWalkPosition() { return targetWalkPosition; }
	TilePosition getTilePosition() { return tilePosition; }
	TilePosition getTargetTilePosition() { return targetTilePosition; }
	
	void setStrength(double newStrength) { strength = newStrength; }
	void setMaxStrength(double newMaxStrength){ maxStrength = newMaxStrength; }
	void setLocal(double newLocal) { local = newLocal; }
	void setGroundRange(double newGroundRange) { groundRange = newGroundRange; }
	void setAirRange(double newAirRange) { airRange = newAirRange; }
	void setPriority(double newPriority) { priority = newPriority; }
	void setGroundDamage(double newGroundDamage) { groundDamage = newGroundDamage; }
	void setAirDamage(double newAirDamage) { airDamage = newAirDamage; }
	void setSpeed(double newSpeed) { speed = newSpeed; }
	void setDeadFrame(int newDeadFrame) { deadFrame = newDeadFrame; }
	void setStrategy(int newStrategy){ strategy = newStrategy; }
	void setLastAttackFrame(int newAttackFrame) { lastAttackFrame = newAttackFrame; }
	void setCurrentGoal(int newGoal){ currentGoal = newGoal; }

	void setUnit(Unit newUnit) { thisUnit = newUnit; }
	void setTarget(Unit newTarget){ target = newTarget; }
	void setTransport(Unit newTransport) { transport = newTransport; }
	void setUnitType(UnitType newType) { unitType = newType; }
	void setCommand(UnitCommandType newCommand) { command = newCommand; }

	void setPosition(Position newPosition){ position = newPosition; }
	void setTargetPosition(Position newPosition) { targetPosition = newPosition; }	
	void setWalkPosition(WalkPosition newWalkPosition) { walkPosition = newWalkPosition; }
	void setTargetWalkPosition(WalkPosition newWalkPosition) { targetWalkPosition = newWalkPosition; }
	void setTilePosition(TilePosition newTilePosition) { tilePosition = newTilePosition; }
	void setTargetTilePosition(TilePosition newTilePosition) { targetTilePosition = newTilePosition; }	
};
