#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class ProbeInfo{
	Unit resource, target, storedUnit;
	WalkPosition miniTile;
	Position resourcePosition;
	int lastGatherFrame;
public:
	// Constructors
	ProbeInfo();

	// Unit access
	Unit unit() { return storedUnit; }
	void setUnit(Unit newUnit) { storedUnit = newUnit; }

	// Accessors
	Unit getTarget() { return target; }
	Unit getResource() { return resource; }
	WalkPosition getMiniTile() { return miniTile; }
	Position getResourcePosition() { return resourcePosition; }
	int getLastGatherFrame() { return lastGatherFrame; }

	// Mutators
	void setTarget(Unit newTarget) { target = newTarget; }
	void setResource(Unit newResource) { resource = newResource; }
	void setMiniTile(WalkPosition newTile) { miniTile = newTile; }
	void setResourcePosition(Position newPosition) { resourcePosition = newPosition; }
	void setLastGatherFrame(int newFrame) { lastGatherFrame = newFrame; }
};