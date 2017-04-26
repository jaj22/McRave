#include <BWAPI.h>
#include <vector>

using namespace BWAPI;
using namespace std;

// External building positioning variables
extern vector <TilePosition> activeExpansion;
extern vector <TilePosition> nextExpansion;
extern TilePosition playerStartingTilePosition;
extern map <UnitType, pair<TilePosition, Unit>> queuedBuildings;
extern set <BWTA::Region*> allyTerritory;

// External resource variables
extern int queuedMineral, queuedGas, reservedMineral, reservedGas;
extern vector<Unit> geysers;
extern int mineralHeatmap[256][256];

// Build Order variables
extern bool terranBio;
extern map <UnitType, double> unitScore;
extern int supply;
extern bool noZealots;

// Function declarations
TilePosition buildingManager(UnitType building);
bool canBuildHere(UnitType building, TilePosition buildTilePosition);
TilePosition getBuildLocationNear(UnitType building, TilePosition buildTilePosition, bool ignoreCond);
void productionManager(Unit building);
TilePosition cannonManager(TilePosition base, UnitType building);