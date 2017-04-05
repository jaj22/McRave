#include "UnitScore.h"



/*int targetPriority(UnitType unit, UnitType target)
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
}*/

void unitScoreUpdate(UnitType unit, int count)
{
	switch (unit)
	{
		if (Broodwar->enemy()->getRace() == Races::Terran)
		{
	case Enum::Terran_Marine:
		unitScore[Protoss_Zealot] += 0.5 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Terran_Medic:
		unitScore[Protoss_Zealot] += 0.5 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Terran_Firebat:
		unitScore[Protoss_Zealot] += 0.5 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Terran_Vulture:
		unitScore[Protoss_Zealot] += 0.5 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Terran_Goliath:
		unitScore[Protoss_Zealot] += 0.5 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Terran_Wraith:
		unitScore[Protoss_Zealot] += 0 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Terran_Science_Vessel:
		unitScore[Protoss_Zealot] += 0 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Terran_Battlecruiser:
		unitScore[Protoss_Zealot] += 0 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 4 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Terran_Siege_Tank_Siege_Mode:
		unitScore[Protoss_Zealot] += 4 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Terran_Siege_Tank_Tank_Mode:
		unitScore[Protoss_Zealot] += 4 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
		}
		if (Broodwar->enemy()->getRace() == Races::Zerg)
		{
	case Enum::Zerg_Zergling:
		unitScore[Protoss_Zealot] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 0.25 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Zerg_Hydralisk:
		unitScore[Protoss_Zealot] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Zerg_Lurker:
		unitScore[Protoss_Zealot] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 4 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Zerg_Ultralisk:
		unitScore[Protoss_Zealot] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 4 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Zerg_Mutalisk:
		unitScore[Protoss_Zealot] += 0 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Zerg_Guardian:
		unitScore[Protoss_Zealot] += 0 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 4 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Zerg_Defiler:
		unitScore[Protoss_Zealot] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 0 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
		}
		if (Broodwar->enemy()->getRace() == Races::Protoss)
		{
	case Enum::Protoss_Zealot:
		unitScore[Protoss_Zealot] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Protoss_Dragoon:
		unitScore[Protoss_Zealot] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Protoss_Reaver:
		unitScore[Protoss_Zealot] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Protoss_High_Templar:
		unitScore[Protoss_Zealot] += 0.5 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Protoss_Dark_Templar:
		unitScore[Protoss_Zealot] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 1 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Protoss_Archon:
		unitScore[Protoss_Zealot] += 0.5 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Protoss_Scout:
		unitScore[Protoss_Zealot] += 0 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 2 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Protoss_Carrier:
		unitScore[Protoss_Zealot] += 0 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 4 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
	case Enum::Protoss_Arbiter:
		unitScore[Protoss_Zealot] += 0 * count * Broodwar->self()->completedUnitCount(Protoss_Dragoon) / Broodwar->self()->completedUnitCount(Protoss_Zealot);
		unitScore[Protoss_Dragoon] += 0.5 * count * Broodwar->self()->completedUnitCount(Protoss_Zealot) / Broodwar->self()->completedUnitCount(Protoss_Dragoon);
		break;
		}
	}
}