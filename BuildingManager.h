#include <BWAPI.h>
#include "Header.h"

using namespace BWAPI;
extern int nexusDesired;
extern TilePosition playerStartingTilePosition;

void buildingManager(UnitType building, Unit builder, TilePosition nexus);
void nexusManager(UnitType building, Unit builder, TilePosition expansion);
bool canBuildHere(UnitType building, Unit builder, TilePosition buildPosition);
TilePosition getBuildLocationNear(UnitType building, Unit builder, TilePosition buildPosition);