#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class ProbeInfo{
private:
	Unit target;
public:
	// Constructors
	ProbeInfo();
	~ProbeInfo();
	ProbeInfo(Unit target);

	// Accessors
	Unit getTarget() const;

	// Mutators
	void setTarget(Unit newTarget);
};