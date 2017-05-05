#include <BWAPI.h>
#include "Singleton.h"
#include "ProbeInfo.h"

using namespace std;
using namespace BWAPI;

class ProbeTrackerClass
{
	map <Unit, ProbeInfo>& myProbes;
public:
	void storeProbe(Unit);
	void removeProbe(Unit);
	void assignProbe(Unit);
	void update();
};

extern bool scouting;
extern int resourceGrid[256][256];
extern Unit scout;
extern int supply;

typedef Singleton<ProbeTrackerClass> ProbeTracker;