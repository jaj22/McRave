#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class SupportUnitInfo
{
	Position position, destination;
	WalkPosition walkPosition;
	Unit storedUnit, target;
public:
	SupportUnitInfo();

	// Accessors
	Unit unit() { return storedUnit; }
	Position getPosition() { return position; }
	Position getDestination() { return destination; }
	WalkPosition getMiniTile() { return walkPosition; }

	// Mutators
	void setUnit(Unit newUnit) { storedUnit = newUnit; }
	void setPosition(Position newPosition) { position = newPosition; }
	void setDestination(Position newDestination) { destination = newDestination; }
	void setWalkPosition(WalkPosition newWalkPosition) { walkPosition = newWalkPosition; }
};

class TransportInfo
{
	Position position, destination, drop;
	Unit storedUnit, target;
	UnitType transportType;
	WalkPosition walkPosition;
	set <Unit> assignedCargo;
	int loadState;
	int cargoSize;
	bool harassing;

public:
	TransportInfo();
	~TransportInfo();

	// Unit access
	Unit unit() { return storedUnit; }
	void setUnit(Unit newTransport) { storedUnit = newTransport; }

	// Accessors
	Position getDrop() { return drop; }
	Position getPosition() { return position; }
	Position getDestination() { return destination; }
	Unit getTarget() { return target; }
	UnitType getType() { return transportType; }
	WalkPosition getWalkPosition() { return walkPosition; }
	set <Unit>& getAssignedCargo() { return assignedCargo; }
	int getCargoSize() { return cargoSize; }
	int getLoadState() { return loadState; }
	bool isHarassing() { return harassing; }

	// Mutators
	void setDrop(Position newDrop) { drop = newDrop; }
	void setPosition(Position newPosition) { position = newPosition; }
	void setDestination(Position newDestination) { destination = newDestination; }
	void setTarget(Unit newTarget) { target = newTarget; }
	void setType(UnitType newType) { transportType = newType; }
	void setWalkPosition(WalkPosition newWalkPosition) { walkPosition = newWalkPosition; }
	void setCargoSize(int newSize) { cargoSize = newSize; }
	void setLoadState(int newState) { loadState = newState; }
	void setHarassing(bool newState) { harassing = newState; }

	// Functions
	void assignCargo(Unit);
	void removeCargo(Unit);
};