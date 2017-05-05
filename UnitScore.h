#include <BWAPI.h>
#include <BWTA.h>
#include <vector>

using namespace BWAPI;

using namespace std;

//class UnitScore{
//	UnitType unitType;
//	int strength;
//public:
//	UnitScore();
//	UnitScore(UnitType, int);
//
//	UnitType getUnitType() const;
//	UnitType getScore() const;
//	void setUnitType(UnitType);
//	void setScore(int);
//};

extern map <UnitType, double> unitScore;
//extern map <UnitType, UnitScore> unitTargets;

void unitScoreUpdate(UnitType unit, int count);