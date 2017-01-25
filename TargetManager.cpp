#include "TargetManager.h"

using namespace BWAPI;

Unit targetPriority(Unit unit)
{
	// If unit up to 5 tiles away from current unit
	if (unit->getUnitsInRadius(unit->getType().groundWeapon().maxRange() + 64, Filter::IsEnemy && (Filter::GetType == UnitTypes::Terran_Medic || Filter::GetType == UnitTypes::Zerg_Lurker || Filter::GetType == UnitTypes::Protoss_Dark_Templar)).size() > 0)
	{
		return unit->getClosestUnit(Filter::IsEnemy && (Filter::GetType == UnitTypes::Terran_Medic || Filter::GetType == UnitTypes::Zerg_Lurker || Filter::GetType == UnitTypes::Protoss_Dark_Templar));
	}
	else
	{
		return unit->getClosestUnit(Filter::IsEnemy && Filter::IsVisible
			&& Filter::IsDetected && (!Filter::IsBuilding || Filter::GetType == UnitTypes::Enum::Protoss_Photon_Cannon
			|| Filter::GetType == UnitTypes::Enum::Terran_Bunker
			|| Filter::GetType == UnitTypes::Enum::Zerg_Sunken_Colony
			|| Filter::GetType == UnitTypes::Enum::Zerg_Spore_Colony));
	}
}