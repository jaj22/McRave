#include "TargetManager.h"

map <UnitType, int> zealotTargets;
map <UnitType, int> dragoonTargets;

int targetPriority(UnitType unit, UnitType target)
{
	Race enemy = Broodwar->enemy()->getRace();
	switch (enemy)
	{
	case Races::Enum::Terran:
		zealotTargets.emplace(Terran_SCV, 1);
		zealotTargets.emplace(Terran_Marine, 3);
		zealotTargets.emplace(Terran_Firebat, 2);
		zealotTargets.emplace(Terran_Medic, 4);
		zealotTargets.emplace(Terran_Ghost, 5);
		zealotTargets.emplace(Terran_Vulture, 0);
		zealotTargets.emplace(Terran_Goliath, 8);
		zealotTargets.emplace(Terran_Siege_Tank_Tank_Mode, 10);
		zealotTargets.emplace(Terran_Siege_Tank_Siege_Mode, 10);
		zealotTargets.emplace(Terran_Wraith, 0);
		zealotTargets.emplace(Terran_Dropship, 0);
		zealotTargets.emplace(Terran_Science_Vessel, 0);
		zealotTargets.emplace(Terran_Battlecruiser, 0);
		zealotTargets.emplace(Terran_Valkyrie, 0);
		zealotTargets.emplace(Terran_Bunker, 5);

		dragoonTargets.emplace(Terran_SCV, 1);
		dragoonTargets.emplace(Terran_Marine, 2);
		dragoonTargets.emplace(Terran_Firebat, 3);
		dragoonTargets.emplace(Terran_Medic, 4);
		dragoonTargets.emplace(Terran_Ghost, 5);
		dragoonTargets.emplace(Terran_Vulture, 8);
		dragoonTargets.emplace(Terran_Goliath, 8);
		dragoonTargets.emplace(Terran_Siege_Tank_Tank_Mode, 10);
		dragoonTargets.emplace(Terran_Siege_Tank_Siege_Mode, 10);
		dragoonTargets.emplace(Terran_Wraith, 7);
		dragoonTargets.emplace(Terran_Dropship, 9);
		dragoonTargets.emplace(Terran_Science_Vessel, 10);
		dragoonTargets.emplace(Terran_Battlecruiser, 10);
		dragoonTargets.emplace(Terran_Valkyrie, 7);
		dragoonTargets.emplace(Terran_Bunker, 5);


		break;
	case Races::Enum::Protoss:
		break;
	}
	

	
}
