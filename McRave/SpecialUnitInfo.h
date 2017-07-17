#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class SupportUnitInfo
{
	Position position, destination;
	WalkPosition walkPosition;
	Unit thisUnit, target;
public:
	SupportUnitInfo();

	Unit unit() { return thisUnit; }
	Position getPosition() { return position; }
	Position getDestination() { return destination; }
	WalkPosition getWalkPosition() { return walkPosition; }
	void setUnit(Unit newUnit) { thisUnit = newUnit; }
	void setTarget(Unit newUnit) { target = newUnit; }
	void setPosition(Position newPosition) { position = newPosition; }
	void setDestination(Position newPosition) { destination = newPosition; }
	void setWalkPosition(WalkPosition newWalkPosition) { walkPosition = newWalkPosition; }
};

class TransportInfo
{
	int loadState, cargoSize;
	bool harassing;
	Unit thisUnit, target;
	UnitType transportType;
	set <Unit> assignedCargo;
	Position position, destination, drop;
	WalkPosition walkPosition;
public:
	TransportInfo();
	~TransportInfo();

	int getCargoSize() { return cargoSize; }
	int getLoadState() { return loadState; }
	bool isHarassing() { return harassing; }
	Unit unit() { return thisUnit; }
	Unit getTarget() { return target; }
	UnitType getType() { return transportType; }
	set <Unit>& getAssignedCargo() { return assignedCargo; }
	Position getDrop() { return drop; }
	Position getPosition() { return position; }
	Position getDestination() { return destination; }
	WalkPosition getWalkPosition() { return walkPosition; }

	void setCargoSize(int newSize) { cargoSize = newSize; }
	void setLoadState(int newState) { loadState = newState; }
	void setHarassing(bool newState) { harassing = newState; }
	void setUnit(Unit newTransport) { thisUnit = newTransport; }
	void setTarget(Unit newTarget) { target = newTarget; }
	void setType(UnitType newType) { transportType = newType; }
	void setDrop(Position newDrop) { drop = newDrop; }
	void setPosition(Position newPosition) { position = newPosition; }
	void setDestination(Position newPosition) { destination = newPosition; }
	void setWalkPosition(WalkPosition newWalkPosition) { walkPosition = newWalkPosition; }

	// Add cargo to the assigned cargo set
	void assignCargo(Unit);

	// Remove cargo from the assigned cargo set
	void removeCargo(Unit);
};