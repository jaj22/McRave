#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class ProbeInfo{
private:
	Unit target;
	WalkPosition miniTile;
public:
	// Constructors
	ProbeInfo();
	~ProbeInfo();

	// Accessors
	Unit getTarget() {	return target;	}
	WalkPosition getMiniTile() { return miniTile; }

	// Mutators
	void setTarget(Unit newTarget) { target = newTarget; }
	void setMiniTile(WalkPosition newTile) { miniTile = newTile; }
};