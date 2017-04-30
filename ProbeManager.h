#include <BWAPI.h>

using namespace std;
using namespace BWAPI;


class ProbeInfo{
private:
	Unit target;
public:
	// Constructors
	ProbeInfo();
	~ProbeInfo();
	ProbeInfo(Unit target);

	// Accessors
	Unit getTarget() const;

	// Mutators
	void setTarget(Unit newTarget);
};


void storeProbe(Unit probe, map <Unit, ProbeInfo>& myProbes);

// External Variables
extern vector<Unit> combatProbe;
extern bool saturated, gasNeeded;
//extern map <Unit, ResourceInfo> myMinerals;
//extern map <Unit, ResourceInfo> myGas;


// Functions
Unit assignProbe(Unit probe);
void assignCombat(Unit probe);
void unAssignCombat(Unit probe);