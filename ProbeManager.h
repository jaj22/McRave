#include <BWAPI.h>

void assignProbe(BWAPI::Unit probe);
using namespace std;
using namespace BWAPI;

// Probe variables
extern vector<int> probeID;
extern vector<int> gasWorkerID;
extern vector<int> mineralWorkerID;
extern vector<int> deadProbeID;

// Resource variables (externally from global.h)
extern vector<int> mineralID;
extern vector<TilePosition> gasTilePosition;
extern vector<int> assimilatorID;