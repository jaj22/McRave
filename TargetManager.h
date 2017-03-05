#include <BWAPI.h>
#include <BWTA.h>
#include <vector>

using namespace BWAPI;

extern Position playerStartingPosition;

Unit targetPriority(Unit unit);
Unit groundTargetPriority(Unit unit);
Unit clusterTargetPriority(Unit unit);