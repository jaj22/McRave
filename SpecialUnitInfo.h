#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;
using namespace UnitTypes;

class SpecialUnitInfoClass
{
	Position position, destination;
	WalkPosition miniTile;
	set <Unit> reaverSet;
	Unit shuttlePair;
public:
	SpecialUnitInfoClass();
	~SpecialUnitInfoClass();

	Position getPosition() { return position; }
	Position getDestination() { return destination; }
	WalkPosition getMiniTile() { return miniTile; }
	set <Unit>& getReavers() { return reaverSet; }
	Unit getShuttle() { return shuttlePair; }

	void setPosition(Position newPosition) { position = newPosition; }
	void setDestination(Position newDestination) { destination = newDestination; }
	void setMiniTile(WalkPosition newMiniTile) { miniTile = newMiniTile; }
	void addReaver(Unit newReaver) { reaverSet.emplace(newReaver);	}
	void removeReaver(Unit oldReaver) { reaverSet.erase(oldReaver); }
	void setShuttle(Unit newShuttle) { shuttlePair = newShuttle; }
};