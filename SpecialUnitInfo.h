#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class SupportUnitInfo
{
	Position position, destination;
	WalkPosition miniTile;

public:
	SupportUnitInfo();
	~SupportUnitInfo();

	// Accessors
	Position getPosition() { return position; }
	Position getDestination() { return destination; }
	WalkPosition getMiniTile() { return miniTile; }

	// Mutators
	void setPosition(Position newPosition) { position = newPosition; }
	void setDestination(Position newDestination) { destination = newDestination; }
	void setMiniTile(WalkPosition newMiniTile) { miniTile = newMiniTile; }
};

class TransportInfo
{
	Position position, destination, drop;
	Unit transport, target;
	UnitType transportType;
	WalkPosition miniTile;
	set <Unit> assignedCargo;
	int loadState; // Tristate: Loading, unloading, nothing (0,1,2)
	int cargoSize;
	bool harassing;

public:
	TransportInfo();
	~TransportInfo();

	// Unit access
	Unit unit() { return transport; }
	void setUnit(Unit newTransport) { transport = newTransport; }

	// Accessors
	Position getDrop() { return drop; }
	Position getPosition() { return position; }
	Position getDestination() { return destination; }
	Unit getTarget() { return target; }
	UnitType getType() { return transportType; }
	WalkPosition getMiniTile() { return miniTile; }
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
	void setMiniTile(WalkPosition newMiniTile) { miniTile = newMiniTile; }
	void setCargoSize(int newSize) { cargoSize = newSize; }
	void setLoadState(int newState) { loadState = newState; }
	void setHarassing(bool newState) { harassing = newState; }

	// Functions
	void assignCargo(Unit);
	void removeCargo(Unit);
};