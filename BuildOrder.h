#include <BWAPI.h>
#include <vector>

using namespace std;
using namespace BWAPI;

// External building variables
extern int nexusDesired;
extern int pylonDesired;
extern int gasDesired;
extern int gateDesired;
extern int forgeDesired;
extern int coreDesired;

// External advanced building variables
extern int roboDesired;
extern int stargateDesired;
extern int citadelDesired;
extern int supportBayDesired;
extern int fleetBeaconDesired;
extern int archivesDesired;
extern int observatoryDesired;
extern int tribunalDesired;

// External resource and expansion variables
extern vector<TilePosition>gasTilePosition;
extern vector<int> mineralID, mineralWorkerID;
extern vector<TilePosition> nextExpansion;

// Function declaration
void getBuildOrder();