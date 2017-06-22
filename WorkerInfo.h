#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class WorkerInfo{
	int lastGatherFrame;
	Unit thisUnit, target, resource;
	WalkPosition walkPosition;
	Position position;
	TilePosition tilePosition;
public:
	WorkerInfo();

	int getLastGatherFrame() { return lastGatherFrame; }

	Unit unit() { return thisUnit; }
	Unit getTarget() { return target; }
	Unit getResource() { return resource; }

	Position getPosition() { return position; }
	WalkPosition getWalkPosition() { return walkPosition; }
	TilePosition getTilePosition() { return tilePosition; }	

	void setLastGatherFrame(int newFrame) { lastGatherFrame = newFrame; }

	void setUnit(Unit newUnit) { thisUnit = newUnit; }
	void setTarget(Unit newTarget) { target = newTarget; }
	void setResource(Unit newResource) { resource = newResource; }

	void setPosition(Position newPosition) { position = newPosition; }
	void setWalkPosition(WalkPosition newPosition) { walkPosition = newPosition; }
	void setTilePosition(TilePosition newPosition) { tilePosition = newPosition; }
	
};