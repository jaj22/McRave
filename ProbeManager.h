#include <BWAPI.h>

using namespace std;
using namespace BWAPI;

// Probe variables
extern vector<int> probeID;
extern vector<int> gasWorkerID;
extern vector<int> mineralWorkerID;
extern vector<int> buildingWorkerID;
extern vector<int> scoutWorkerID;
extern vector<int> additionalMineralWorkerID;
extern vector<int> deadProbeID;
extern vector<int> combatWorkerID;

// Resource variables (externally from global.h)
extern vector<int> mineralID;
extern vector<TilePosition> gasTilePosition;
extern vector<int> assimilatorID;

//Other
extern double enemyStrength, allyStrength;
extern int enemyCountNearby;
extern vector<TilePosition> activeExpansion;

// Unsorted
extern map <Unit, Unit> gasProbeMap;
extern map <Unit, Unit> mineralProbeMap;
extern map <Unit, int> gasMap;
extern map <Unit, int> mineralMap;

// Functions
void assignProbe(BWAPI::Unit probe);