#include <BWAPI.h>


using namespace std;
using namespace BWAPI;

// Probe variables
extern vector<int> probeID;
extern vector<int> gasWorkerID;
extern vector<int> mineralWorkerID;
extern vector<int> deadProbeID;
extern vector<int> combatWorkerID;

// Resource variables (externally from global.h)
extern vector<int> mineralID;
extern vector<TilePosition> gasTilePosition;
extern vector<int> assimilatorID;

//Other
extern int allySupply;
extern int enemyCountNearby;

// Functions
void assignProbe(BWAPI::Unit probe);