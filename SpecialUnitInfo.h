#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class SpecialUnitInfoClass
{
	Position position, destination;
	WalkPosition miniTile;
public:
	SpecialUnitInfoClass();
	~SpecialUnitInfoClass();

	Position getPosition() { return position; }
	Position getDestination() { return destination; }
	WalkPosition getMiniTile() { return miniTile; }

	void setPosition(Position newPosition) { position = newPosition; }
	void setDestination(Position newDestination) { destination = newDestination; }
	void setMiniTile(WalkPosition newMiniTile) { miniTile = newMiniTile; }
};