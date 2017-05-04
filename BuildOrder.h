#include <BWAPI.h>
#include <vector>


using namespace std;
using namespace BWAPI;

// External building variables
extern map <UnitType, int> buildingDesired;
extern map <int, UnitType> idleGates;

// External resource and expansion variables
extern vector<Unit> geysers;
extern vector<TilePosition> nextExpansion;
extern vector<Position>enemyBasePositions;
extern int supply; 

// External strategy paramaters
extern int forceExpand, inactiveNexusCnt;
extern vector<TilePosition> activeExpansion;
extern bool saturated, gasNeeded, noZealots;

// External enemy build tracking
extern bool scouting, terranBio;
extern string currentStrategy;

// Function declaration
void getBuildOrder();
void earlyBuilds(int whichBuild);
void midBuilds(int whichBuild);
void lateBuilds(int whichBuild);
