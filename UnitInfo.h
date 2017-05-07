#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

// Class for storing information about all units
class UnitInfoClass {
	UnitType unitType, targetType;
	Position unitPosition, targetPosition;
	double unitStrength, unitMaxStrength, unitLocal, unitRange;
	UnitCommandType unitCommand;
	Unit target;
	int deadFrame, strategy, lastCommandFrame;

public:
	UnitInfoClass();
	UnitInfoClass(UnitType, Position, double, double, double, UnitCommandType, int, int, int, Unit);
	~UnitInfoClass();

	// Accessors
	UnitType getUnitType() const;
	Position getPosition() const;
	Position getTargetPosition() const;
	double getStrength() const;
	double getMaxStrength() const;
	double getLocal() const;
	double getRange() const;
	UnitCommandType getCommand() const;
	Unit getTarget() const;
	int getDeadFrame() const;
	int getStrategy() const;
	int getLastCommandFrame() const;
	bool hasStim() const;

	// Mutators
	void setUnitType(UnitType);
	void setPosition(Position);
	void setTargetPosition(Position);
	void setStrength(double);
	void setMaxStrength(double);
	void setLocal(double);
	void setRange(double);
	void setCommand(UnitCommandType);
	void setTarget(Unit);
	void setDeadFrame(int);
	void setStrategy(int);
	void setLastCommandFrame(int);
	void setStim(bool);
};