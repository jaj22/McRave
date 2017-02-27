#include "TargetManager.h"

using namespace BWAPI;
using namespace std;


Unit targetPriority(Unit unit)
{
	int searchRadius;
	// If unit is melee, search up to 10 tiles for a target
	if (unit->getType().groundWeapon().maxRange() < 64)
	{
		searchRadius = 32;
	}
	// Else just search up to a tile away
	else
	{
		searchRadius = 64;
	}
	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		searchRadius = 128;
	}
	// If unit up to 5 tiles away from current unit
	if (unit->getUnitsInRadius(unit->getType().groundWeapon().maxRange() + searchRadius, Filter::IsEnemy && Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode).size() > 0)
	{
		return unit->getClosestUnit(Filter::IsEnemy && (Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode));
	}
	else if (unit->getUnitsInRadius(unit->getType().groundWeapon().maxRange() + searchRadius, Filter::IsEnemy && Filter::Exists && (Filter::GetType == UnitTypes::Terran_Medic || (Filter::GetType == UnitTypes::Zerg_Lurker && Filter::IsDetected) || Filter::GetType == UnitTypes::Protoss_Dark_Templar && Filter::IsDetected)).size() > 0)
	{
		return unit->getClosestUnit(Filter::IsEnemy && (Filter::GetType == UnitTypes::Terran_Medic || (Filter::GetType == UnitTypes::Zerg_Lurker && Filter::IsDetected) || Filter::GetType == UnitTypes::Protoss_Dark_Templar && Filter::IsDetected));
	}
	else if (unit->getUnitsInRadius(unit->getType().groundWeapon().maxRange() + searchRadius, Filter::IsEnemy && Filter::Exists && Filter::GetType == UnitTypes::Terran_SCV && Filter::IsRepairing || Filter::GetType == UnitTypes::Terran_SCV && Filter::IsConstructing).size() > 0)
	{
		return unit->getClosestUnit(Filter::IsEnemy && (Filter::GetType == UnitTypes::Terran_SCV && Filter::IsRepairing || Filter::GetType == UnitTypes::Terran_SCV && Filter::IsConstructing));
	}
	else if (unit->getUnitsInRadius(320, Filter::IsEnemy && Filter::IsVisible && Filter::IsDetected && Filter::GetType != UnitTypes::Zerg_Larva && Filter::GetType != UnitTypes::Zerg_Egg
		&& (!Filter::IsBuilding || Filter::GetType == UnitTypes::Enum::Protoss_Photon_Cannon
		|| Filter::GetType == UnitTypes::Enum::Terran_Missile_Turret
		|| Filter::GetType == UnitTypes::Enum::Terran_Bunker
		|| Filter::GetType == UnitTypes::Enum::Zerg_Sunken_Colony
		|| Filter::GetType == UnitTypes::Enum::Zerg_Spore_Colony)).size() > 0)
	{
		
		return unit->getClosestUnit(Filter::IsEnemy && Filter::IsVisible && Filter::IsDetected && Filter::GetType != UnitTypes::Zerg_Larva && Filter::GetType != UnitTypes::Zerg_Egg
			&& (!Filter::IsBuilding || Filter::GetType == UnitTypes::Enum::Protoss_Photon_Cannon
			|| Filter::GetType == UnitTypes::Enum::Terran_Missile_Turret
			|| Filter::GetType == UnitTypes::Enum::Zerg_Sunken_Colony
			|| Filter::GetType == UnitTypes::Enum::Zerg_Spore_Colony));
	}
	else if (Broodwar->self()->supplyUsed() < 100)
	{
		return Broodwar->getClosestUnit(playerStartingPosition, Filter::IsEnemy && Filter::IsDetected);
	}
	else
	{
		return unit->getClosestUnit(Filter::IsEnemy && Filter::IsDetected);
	}
}

Unit clusterTargetPriority(Unit unit)
{
	int highest = 0;
	int range = unit->getType().groundWeapon().maxRange();
	map <int, int> clusters;
	for (Unit u : unit->getUnitsInRadius(range, Filter::IsEnemy))
	{
		if (u->getUnitsInRadius(u->getType().groundWeapon().innerSplashRadius(), Filter::IsEnemy).size() > 0)
		{
			clusters.emplace(u->getID(), u->getUnitsInRadius(u->getType().groundWeapon().innerSplashRadius(), Filter::IsEnemy).size());
		}	
	}
	for (auto c : clusters)
	{	
		if (c.second > highest)
		{
			highest = c.first;
		}
	}
	// If found no clusters (literally no enemy unit around)
	if (highest == 0)
	{
		return NULL;
	}
	return Broodwar->getUnit(highest);
}