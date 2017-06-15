#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class ProbeInfo{
	Unit resource, target;
	WalkPosition miniTile;
	int lastGatherFrame;
public:
	// Constructors
	ProbeInfo();

	// Accessors
	Unit getTarget() { return target; }
	Unit getResource() { return resource; }
	WalkPosition getMiniTile() { return miniTile; }
	int getLastGatherFrame() { return lastGatherFrame; }

	// Mutators
	void setTarget(Unit newTarget) { target = newTarget; }
	void setResource(Unit newResource) { resource = newResource; }
	void setMiniTile(WalkPosition newTile) { miniTile = newTile; }
	void setLastGatherFrame(int newFrame) { lastGatherFrame = newFrame; }
};