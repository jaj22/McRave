// Include API files
#include <BWAPI.h>
#include <BWTA.h>
#include "src\bwem.h"

#include "Singleton.h"

// Include other source files
#include "BuildingManager.h"
#include "ProductionManager.h"
#include "BuildOrder.h"
#include "ResourceManager.h"
#include "ProbeManager.h"
#include "UnitManager.h"
#include "UnitScore.h"
#include "NexusManager.h"


// Include standard libraries that are needed
#include <vector>
#include <iostream>
#include <Windows.h>
#include <set>
#include <iterator>
#include <fstream>
#include <BWTA.h>
#include <utility>



// Namespaces
using namespace BWAPI;
using namespace std;
using namespace BWTA;

// Building Tracker Variables
int queuedMineral = 0, queuedGas = 0, reservedMineral = 0, reservedGas = 0;
int inactiveNexusCnt = 0;

// Building Manager Variables
map <Unit, NexusInfo> myNexus;
map <int, UnitType> idleBuildings;
map <int, TechType> idleTech;
map <int, UpgradeType> idleUpgrade;
map <int, UnitType> idleGates;
map <UnitType, int> buildingDesired;
map <UnitType, pair<TilePosition, Unit>> queuedBuildings;
map <Unit, int> nexusCannonMap;

// Probe Manager Variables
map <Unit, ProbeInfo> myProbes;
Unit scout;
vector<Unit> combatProbe;
bool saturated = false, gasNeeded = false;

// Resource Manager Variables
map <Unit, ResourceInfo> myMinerals;
map <Unit, ResourceInfo> myGas;
vector <Unit> geysers;
vector <Unit> boulders;

// Unit Manager Variables
map <UnitType, double> unitScore;
map <Unit, UnitInfoClass> enemyUnits;
map <Unit, UnitInfoClass> allyUnits;
map <UnitType, int> enemyComposition;
map <UnitType, map<UnitType, int>> unitTargets;
int supply;
vector<int> shuttleID;
vector<int> harassShuttleID;
vector<int> reaverID;
vector<int> harassReaverID;
int aSmall = 0, aMedium = 0, aLarge = 0, eSmall = 0, eMedium = 0, eLarge = 0;
Position supportPosition;

// Strategy Variables
bool terranBio = false;
bool scouting = true;
bool outsideBase = false;
bool wallIn = false;
bool noZealots = false;
int forceExpand = 0;
string currentStrategy;

// Heatmaps
double allyStrength = 0.0, enemyStrength = 0.0;
double enemyGroundStrengthGrid[256][256] = { { 0 } };
double enemyAirStrengthGrid[256][256] = { { 0 } }; 
int shuttleHeatmap[256][256] = { { 0 } };
int enemyGroundClusterGrid[256][256] = { { 0 } };
int enemyAirClusterGrid[256][256] = { { 0 } };
int tankClusterHeatmap[256][256] = { { 0 } };
int allyClusterGrid[256][256] = { { 0 } };
int allyDetectorGrid[256][256] = { { 0 } };
int resourceGrid[256][256] = { { 0 } };

// Terrain Variables
int currentSize = 0;
BWEM::CPPath path;
set <BWTA::Region*> territory;
set <BWTA::Region*> allyTerritory;
vector<BWTA::Region> enemyTerritory;
vector<Position> defendHere;
vector<Position> enemyBasePositions;
vector<TilePosition> nextExpansion;
vector<TilePosition> activeExpansion;
Position enemyStartingPosition, playerStartingPosition;
TilePosition enemyStartingTilePosition, playerStartingTilePosition;
