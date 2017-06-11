#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;
using namespace UnitTypes;

class ProbeInfo{
private:
	Unit resource, target;
	WalkPosition miniTile;
	int lastGatherFrame;
public:
	// Constructors
	ProbeInfo();

	// Accessors
	Unit getTarget() {	return target;	}
	Unit getResource() { return resource; }
	WalkPosition getMiniTile() { return miniTile; }
	int getLastGatherFrame() { return lastGatherFrame; }

	// Mutators
	void setTarget(Unit newTarget) { target = newTarget; }
	void setResource(Unit newResource) { resource = newResource; }
	void setMiniTile(WalkPosition newTile) { miniTile = newTile; }
	void setLastGatherFrame(int newFrame) { lastGatherFrame = newFrame; }
};