#include <BWAPI.h>
#include <BWTA.h>
#include <vector>
using namespace BWAPI;
using namespace std;

// External position variables
extern vector <Position> enemyBasePositions;
extern vector <Position> nearestChokepointPosition;
extern Position enemyStartingPosition;
extern Position holdingPosition;
extern Position zealotPosition;

// External unit counts
extern int probeCnt, zealotCnt, dragoonCnt, highTemplarCnt, darkTemplarCnt, reaverCnt, archonCnt, darkArchonCnt;
extern int observerCnt, shuttleCnt, scoutCnt, carrierCnt, arbiterCnt, corsairCnt;

// Internal position variables
extern Position currentTargetPosition;
extern Position currentPosition;
extern Position chokepointWrap;
extern Position nextPosition;

// Function declarations
void unitGetCommand(Unit unit);
void carrierGetCommand(Unit unit);
void unitGetTarget(Unit unit);
void shuttleManager(Unit unit);
void observerManager(Unit unit);
void reaverManager(Unit unit);
int commandManager();