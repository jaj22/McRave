#include <BWAPI.h>
#include <vector>
using namespace BWAPI;

// External position variables
extern std::vector <Position> enemyBasePositions;
extern std::vector <Position> nearestChokepointPosition;
extern Position enemyStartingPosition;
extern Position holdingPosition;
extern Position zealotPosition;

// External unit counts
extern int dragoonCnt;
extern int carrierCnt;
extern int darkTemplarCnt;


// Function declarations
void unitGetCommand(Unit unit);
void carrierGetCommand(Unit unit);
void unitGetTarget(Unit unit);
void shuttleManager(Unit unit);
int commandManager();