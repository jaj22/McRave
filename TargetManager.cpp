#include "TargetManager.h"

using namespace BWAPI;

Unit targetPriority(Unit unit)
{
	// If unit up to 5 tiles away from current unit
	if (unit->getUnitsInRadius(unit->getType().groundWeapon().maxRange() + 320, Filter::IsEnemy && Filter::Exists && Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode).size() > 0)
	{
		return unit->getClosestUnit(Filter::IsEnemy && (Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode));
	}
	else if (unit->getUnitsInRadius(unit->getType().groundWeapon().maxRange() + 320, Filter::IsEnemy && Filter::Exists && (Filter::GetType == UnitTypes::Terran_Medic || (Filter::GetType == UnitTypes::Zerg_Lurker && Filter::IsVisible) || Filter::GetType == UnitTypes::Protoss_Dark_Templar)).size() > 0)
	{
		return unit->getClosestUnit(Filter::IsEnemy && (Filter::GetType == UnitTypes::Terran_Medic || (Filter::GetType == UnitTypes::Zerg_Lurker && Filter::IsVisible) || Filter::GetType == UnitTypes::Protoss_Dark_Templar));
	}
	else if (unit->getUnitsInRadius(unit->getType().groundWeapon().maxRange() + 320, Filter::IsEnemy && Filter::Exists && Filter::GetType == UnitTypes::Terran_SCV && Filter::IsRepairing || Filter::GetType == UnitTypes::Terran_SCV && Filter::IsConstructing).size() > 0)
	{
		return unit->getClosestUnit(Filter::IsEnemy && (Filter::GetType == UnitTypes::Terran_SCV && Filter::IsRepairing || Filter::GetType == UnitTypes::Terran_SCV && Filter::IsConstructing));
	}
	else
	{
		return unit->getClosestUnit(Filter::IsEnemy && Filter::IsVisible
			&& (!Filter::IsBuilding || Filter::GetType == UnitTypes::Enum::Protoss_Photon_Cannon
			|| Filter::GetType == UnitTypes::Enum::Terran_Bunker
			|| Filter::GetType == UnitTypes::Enum::Terran_Missile_Turret
			|| Filter::GetType == UnitTypes::Enum::Zerg_Sunken_Colony
			|| Filter::GetType == UnitTypes::Enum::Zerg_Spore_Colony));
	}
	return unit->getClosestUnit(Filter::IsEnemy);
}