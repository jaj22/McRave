#include <BWAPI.h>
#include <BWTA.h>
#include <vector>

using namespace BWAPI;
using namespace UnitTypes;
using namespace std;

extern map <UnitType, double> unitScore;

void unitScoreUpdate(UnitType unit, int count);