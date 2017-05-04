#include "UnitScore.h"


//int targetPriority(UnitType unit, UnitType target)
//{
//
//
//
//
//
//
//
//
//	Race enemy = Broodwar->enemy()->getRace();
//	switch (enemy)
//	{
//	case Races::Enum::Terran:
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_SCV, 2));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Marine, 3));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Firebat, 2));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Medic, 4));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Ghost, 5));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Vulture, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Goliath, 8));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Siege_Tank_Tank_Mode, 10));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Siege_Tank_Siege_Mode, 10));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Wraith, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Dropship, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Science_Vessel, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Battlecruiser, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Valkyrie, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Terran_Bunker, 5));
//
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_SCV, 2));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Marine, 2));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Firebat, 3));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Medic, 4));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Ghost, 5));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Vulture, 8));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Goliath, 8));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Siege_Tank_Tank_Mode, 10));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Siege_Tank_Siege_Mode, 10));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Wraith, 7));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Dropship, 9));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Science_Vessel, 10));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Battlecruiser, 10));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Valkyrie, 7));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Terran_Bunker, 5));
//
//		/*reaverTargets.emplace(Terran_SCV, 5);		
//		reaverTargets.emplace(Terran_Marine, 7);
//		reaverTargets.emplace(Terran_Firebat, 6);
//		reaverTargets.emplace(Terran_Medic, 8);
//		reaverTargets.emplace(Terran_Ghost, 8);
//		reaverTargets.emplace(Terran_Vulture, 10);
//		reaverTargets.emplace(Terran_Goliath, 8);
//		reaverTargets.emplace(Terran_Siege_Tank_Tank_Mode, 9);
//		reaverTargets.emplace(Terran_Siege_Tank_Siege_Mode, 9);
//		reaverTargets.emplace(Terran_Wraith, 0);
//		reaverTargets.emplace(Terran_Dropship, 0);
//		reaverTargets.emplace(Terran_Science_Vessel, 0);
//		reaverTargets.emplace(Terran_Battlecruiser, 0);
//		reaverTargets.emplace(Terran_Valkyrie, 0);
//		reaverTargets.emplace(Terran_Bunker, 7);
//
//		archonTargets.emplace(Terran_SCV, 3);
//		archonTargets.emplace(Terran_Marine, 7);
//		archonTargets.emplace(Terran_Firebat, 6);
//		archonTargets.emplace(Terran_Medic, 8);
//		archonTargets.emplace(Terran_Ghost, 8);
//		archonTargets.emplace(Terran_Vulture, 10);
//		archonTargets.emplace(Terran_Goliath, 8);
//		archonTargets.emplace(Terran_Siege_Tank_Tank_Mode, 9);
//		archonTargets.emplace(Terran_Siege_Tank_Siege_Mode, 9);
//		archonTargets.emplace(Terran_Wraith, 6);
//		archonTargets.emplace(Terran_Dropship, 6);
//		archonTargets.emplace(Terran_Science_Vessel, 10);
//		archonTargets.emplace(Terran_Battlecruiser, 10);
//		archonTargets.emplace(Terran_Valkyrie, 0);
//		archonTargets.emplace(Terran_Bunker, 8);*/
//		break;
//	case Races::Enum::Protoss:
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Probe, 3));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Zealot, 7));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Dragoon, 2));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_High_Templar, 10));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Dark_Templar, 6));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Reaver, 8));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Archon, 4));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Dark_Archon, 9));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Observer, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Shuttle, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Scout, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Carrier, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Interceptor, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Arbiter, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Corsair, 0));
//		unitTargets.emplace(Protoss_Zealot, make_pair(Protoss_Photon_Cannon, 5));
//
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Probe, 3));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Zealot, 5));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Dragoon, 8));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_High_Templar, 7));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Dark_Templar, 9));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Reaver, 10));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Archon, 6));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Dark_Archon, 9));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Observer, 7));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Shuttle, 7));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Scout, 2));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Carrier, 10));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Interceptor, 1));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Arbiter, 10));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Corsair, 1));
//		unitTargets.emplace(Protoss_Dragoon, make_pair(Protoss_Photon_Cannon, 8));
//
//		/*reaverTargets.emplace(Protoss_Probe, 5);
//		reaverTargets.emplace(Protoss_Zealot, 7);
//		reaverTargets.emplace(Protoss_Dragoon, 9);
//		reaverTargets.emplace(Protoss_High_Templar, 6);
//		reaverTargets.emplace(Protoss_Dark_Templar, 6);
//		reaverTargets.emplace(Protoss_Reaver, 10);
//		reaverTargets.emplace(Protoss_Archon, 7);
//		reaverTargets.emplace(Protoss_Dark_Archon, 9);
//		reaverTargets.emplace(Protoss_Observer, 0);
//		reaverTargets.emplace(Protoss_Shuttle, 0);
//		reaverTargets.emplace(Protoss_Scout, 0);
//		reaverTargets.emplace(Protoss_Carrier, 0);
//		reaverTargets.emplace(Protoss_Interceptor, 0);
//		reaverTargets.emplace(Protoss_Arbiter, 0);
//		reaverTargets.emplace(Protoss_Corsair, 0);
//		reaverTargets.emplace(Protoss_Photon_Cannon, 8);
//
//		archonTargets.emplace(Protoss_Probe, 5);
//		archonTargets.emplace(Protoss_Zealot, 7);
//		archonTargets.emplace(Protoss_Dragoon, 9);
//		archonTargets.emplace(Protoss_High_Templar, 6);
//		archonTargets.emplace(Protoss_Dark_Templar, 6);
//		archonTargets.emplace(Protoss_Reaver, 10);
//		archonTargets.emplace(Protoss_Archon, 7);
//		archonTargets.emplace(Protoss_Dark_Archon, 9);
//		archonTargets.emplace(Protoss_Observer, 0);
//		archonTargets.emplace(Protoss_Shuttle, 0);
//		archonTargets.emplace(Protoss_Scout, 0);
//		archonTargets.emplace(Protoss_Carrier, 0);
//		archonTargets.emplace(Protoss_Interceptor, 0);
//		archonTargets.emplace(Protoss_Arbiter, 0);
//		archonTargets.emplace(Protoss_Corsair, 0);
//		archonTargets.emplace(Protoss_Photon_Cannon, 8);*/
//		break;
//	}
//}

void unitScoreUpdate(UnitType unit, int count)
{
	switch (unit)
	{
		if (Broodwar->enemy()->getRace() == Races::Terran)
		{
	case Enum::Terran_Marine:
		unitScore[Protoss_Zealot] += max(0.0, log(1+(0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1+(0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Medic:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Firebat:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Vulture:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Goliath:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Wraith:
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Science_Vessel:
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Battlecruiser:
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (1.00*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Siege_Tank_Siege_Mode:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.85*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.15*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Siege_Tank_Tank_Mode:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.85*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.15*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
		}
		if (Broodwar->enemy()->getRace() == Races::Zerg)
		{
	case Enum::Zerg_Zergling:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Hydralisk:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Lurker:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Ultralisk:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Mutalisk:
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Guardian:
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Defiler:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
		}
		if (Broodwar->enemy()->getRace() == Races::Protoss)
		{

		}
	}
}