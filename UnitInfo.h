#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

// Class for storing information about all units
class UnitInfoClass {
	double strength, maxStrength, local, range, priority, speed;
	int deadFrame, strategy, lastAttackFrame;
	Position position, targetPosition;
	WalkPosition miniTile;	
	Unit target;
	UnitType type;
	UnitCommandType command;
public:
	UnitInfoClass();
	~UnitInfoClass();

	// Accessors
	UnitType getType(){ return type; }
	Position getPosition(){ return position; }
	Position getTargetPosition() { return targetPosition; }
	double getStrength() { return strength; }
	double getMaxStrength() { return maxStrength; }
	double getLocal() { return local; }
	double getRange() { return range; }
	double getPriority() { return priority; }
	double getSpeed() { return speed; }
	UnitCommandType getCommand() { return command; }
	Unit getTarget() { return target; }
	int getDeadFrame() { return deadFrame; }
	int getStrategy() { return strategy; }
	int getLastAttackFrame() { return lastAttackFrame; }
	WalkPosition getMiniTile() { return miniTile; }

	// Mutators
	void setUnitType(UnitType newType) { type = newType; }
	void setPosition(Position newPosition){ position = newPosition; }
	void setTargetPosition(Position newTargetPosition) { targetPosition = newTargetPosition; }
	void setStrength(double newStrength) { strength = newStrength; }
	void setMaxStrength(double newMaxStrength){ maxStrength = newMaxStrength; }
	void setLocal(double newLocal) { local = newLocal; }
	void setRange(double newRange) { range = newRange; }
	void setPriority(double newPriority) { priority = newPriority; }
	void setSpeed(double newSpeed) { speed = newSpeed; }
	void setCommand(UnitCommandType newCommand) { command = newCommand; }
	void setTarget(Unit newTarget){ target = newTarget; }
	void setDeadFrame(int newDeadFrame) { deadFrame = newDeadFrame; }
	void setStrategy(int newStrategy){ strategy = newStrategy; }
	void setLastAttackFrame(int newAttackFrame) { lastAttackFrame = newAttackFrame; }
	void setMiniTile(WalkPosition newMiniTile) { miniTile = newMiniTile; }
};