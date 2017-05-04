#include <BWAPI.h>
#include "Singleton.h"
#include "ProbeInfo.h"

using namespace std;
using namespace BWAPI;

class ProbeTrackerClass
{

};

void storeProbe(Unit probe, map <Unit, ProbeInfo>& myProbes);

// External Variables
extern vector<Unit> combatProbe;
extern bool saturated, gasNeeded;

// Functions
Unit assignProbe(Unit probe);
void assignCombat(Unit probe);
void unAssignCombat(Unit probe);

typedef Singleton<ProbeTrackerClass> ProbeTracker;