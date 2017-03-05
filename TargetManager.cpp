#include "TargetManager.h"

using namespace BWAPI;
using namespace std;


Unit targetPriority(Unit unit)
{
	int searchRadius = 128;
	
	// If unit up to 5 tiles away from current unit
	if (unit->getUnitsInRadius(unit->getType().groundWeapon().maxRange() + searchRadius, Filter::IsEnemy && Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode).size() > 0)
	{
		return unit->getClosestUnit(Filter::IsEnemy && (Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode));
	}
	else if (unit->getUnitsInRadius(unit->getType().groundWeapon().maxRange(), Filter::IsEnemy && Filter::Exists && (Filter::GetType == UnitTypes::Terran_Medic || (Filter::GetType == UnitTypes::Zerg_Lurker && Filter::IsDetected) || Filter::GetType == UnitTypes::Protoss_Dark_Templar && Filter::IsDetected)).size() > 0)
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
			|| Filter::GetType == UnitTypes::Enum::Terran_Bunker
			|| Filter::GetType == UnitTypes::Enum::Zerg_Sunken_Colony
			|| Filter::GetType == UnitTypes::Enum::Zerg_Spore_Colony));
	}
	else if (Broodwar->self()->supplyUsed() < 100)
	{
		return Broodwar->getClosestUnit(playerStartingPosition, Filter::IsEnemy && Filter::IsDetected);
	}
	return unit->getClosestUnit(Filter::IsEnemy && Filter::IsDetected);
}

Unit groundTargetPriority(Unit unit)
{
	int searchRadius = 128;
	// If unit up to 5 tiles away from current unit
	if (unit->getUnitsInRadius(unit->getType().groundWeapon().maxRange() + searchRadius, Filter::IsEnemy && Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode).size() > 0)
	{
		return unit->getClosestUnit(Filter::IsEnemy && (Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode));
	}
	else if (unit->getUnitsInRadius(unit->getType().groundWeapon().maxRange(), Filter::IsEnemy && Filter::Exists && (Filter::GetType == UnitTypes::Terran_Medic || (Filter::GetType == UnitTypes::Zerg_Lurker && Filter::IsDetected) || Filter::GetType == UnitTypes::Protoss_Dark_Templar && Filter::IsDetected)).size() > 0)
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
			|| Filter::GetType == UnitTypes::Enum::Terran_Bunker
			|| Filter::GetType == UnitTypes::Enum::Zerg_Sunken_Colony
			|| Filter::GetType == UnitTypes::Enum::Zerg_Spore_Colony));
	}
	else if (Broodwar->self()->supplyUsed() < 100)
	{
		return Broodwar->getClosestUnit(playerStartingPosition, Filter::IsEnemy && Filter::IsDetected && !Filter::IsFlyer);
	}
	return unit->getClosestUnit(Filter::IsEnemy && Filter::IsDetected && !Filter::IsFlyer);
}

Unit clusterTargetPriority(Unit unit)
{
	int highest = 0;

	// Radius of psi-storm and stasis
	int radius = 96;
	
	// Smaller radius for clusters
	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		radius = 32;
	}
	
	map <int, int> clusters;
	// Just want Arbiters to target Tanks right now, eventually add others (Ultras, Reavers, Goons, Carriers etc)
	if (unit->getType() == UnitTypes::Protoss_Arbiter)
	{
		// Use increased range for finding tanks
		for (Unit u : unit->getUnitsInRadius(480, Filter::IsEnemy && !Filter::IsStasised && (Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode)))
		{
			if (u->getUnitsInRadius(radius, Filter::IsEnemy && !Filter::IsStasised).size() > 0)
			{
				clusters.emplace(u->getID(), u->getUnitsInRadius(radius, Filter::IsEnemy && !Filter::IsStasised).size());
			}
		}
	}
	else if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		for (Unit u : unit->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsStasised))
		{
			if (u->getUnitsInRadius(radius, Filter::IsEnemy).size() > 0)
			{
				clusters.emplace(u->getID(), u->getUnitsInRadius(radius, Filter::IsEnemy && !Filter::IsStasised).size());
			}
		}
	}
	else if (unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		for (Unit u : unit->getUnitsInRadius(288, Filter::IsEnemy && !Filter::IsStasised && !Filter::IsBuilding))
		{
			if (u->getUnitsInRadius(radius, Filter::IsEnemy).size() > 0)
			{
				clusters.emplace(u->getID(), u->getUnitsInRadius(radius, Filter::IsEnemy && !Filter::IsStasised && !Filter::IsBuilding).size());
			}
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
		return unit->getClosestUnit(Filter::IsEnemy && !Filter::IsFlyer);
	}
	return Broodwar->getUnit(highest);
}