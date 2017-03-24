#include <BWAPI.h>

using namespace std;
using namespace BWAPI;

// External Variables
extern vector<Unit> combatProbe;
extern map <Unit, Unit> mineralProbeMap, gasProbeMap;
extern map <Unit, int> mineralMap, gasMap;
extern bool saturated;

// Functions
void assignProbe(BWAPI::Unit probe);
void assignCombat(Unit probe);
void unAssignCombat(Unit probe);