#include "McRave.h"

double UtilTrackerClass::getStrength(UnitInfo& unit, Player who)
{
	// Some hardcoded values that don't have attacks but should still be considered for strength
	if (unit.getType() == UnitTypes::Terran_Medic)
	{
		return 10.0;
	}
	if (unit.getType() == UnitTypes::Protoss_High_Templar)
	{
		return 20.0;
	}
	if (unit.getType() == UnitTypes::Protoss_Scarab || unit.getType() == UnitTypes::Terran_Vulture_Spider_Mine || unit.getType() == UnitTypes::Zerg_Egg || unit.getType() == UnitTypes::Zerg_Larva || unit.getType() == UnitTypes::Protoss_Interceptor)
	{
		return 0.0;
	}

	double range, damage, speed;
	range = unit.getGroundRange();
	if (unit.getType().groundWeapon().damageCooldown() > 0)
	{
		damage = unit.getGroundDamage() / double(unit.getType().groundWeapon().damageCooldown());
	}
	else
	{
		damage = unit.getGroundDamage();
	}
	speed = unit.getSpeed();

	if (!unit.getType().isWorker() && unit.getGroundDamage() > 0)
	{
		// Check for Zergling attack speed upgrade
		if (unit.getType() == UnitTypes::Zerg_Zergling && who->getUpgradeLevel(UpgradeTypes::Adrenal_Glands))
		{
			damage = damage * 1.33;
		}
		if (speed > 0)
		{
			return range * damage * speed / 1000.0;
		}
		else
		{
			return range * damage / 1000;
		}
	}
	return 0.0;
}

double UtilTrackerClass::getAirStrength(UnitInfo& unit, Player who)
{
	double range, damage, speed;
	range = unit.getAirRange();
	damage = unit.getAirDamage() / double(unit.getType().airWeapon().damageCooldown());
	speed = unit.getSpeed();

	if (!unit.getType().isWorker() && damage > 0)
	{
		if (speed > 0)
		{
			return range * damage * speed / 1000.0;
		}
		else
		{
			return range * damage / 1000.0;
		}
	}
	return 0.0;
}

double UtilTrackerClass::getVisibleStrength(UnitInfo& unit, Player who)
{
	if (unit.unit()->isMaelstrommed() || unit.unit()->isStasised())
	{
		return 0;
	}

	double hp = double(unit.unit()->getHitPoints() + (unit.unit()->getShields())) / double(unit.getType().maxHitPoints() + (unit.getType().maxShields()));

	if (unit.unit()->isCloaked() && !unit.unit()->isDetected())
	{
		return 4.0 * hp * getStrength(unit, who);
	}
	return hp * getStrength(unit, who);
}

double UtilTrackerClass::getPriority(UnitInfo& unit, Player who)
{
	// Low strength units with high threat	
	if (unit.getType() == UnitTypes::Protoss_Arbiter || unit.getType() == UnitTypes::Protoss_Observer || unit.getType() == UnitTypes::Protoss_Shuttle || unit.getType() == UnitTypes::Terran_Science_Vessel || unit.getType() == UnitTypes::Terran_Dropship || unit.getType() == UnitTypes::Terran_Vulture_Spider_Mine)
	{
		return 50.0;
	}
	else if (unit.getType().isWorker())
	{
		return 1.0;
	}
	else
	{
		return unit.getMaxStrength();
	}
}

double UtilTrackerClass::getTrueRange(UnitType unitType, Player who)
{
	// Ranged upgrade check for Dragoons, Marines, Hydralisks and Bunkers

	if (unitType == UnitTypes::Protoss_Dragoon && who->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
	{
		return 192.0;
	}
	else if ((unitType == UnitTypes::Terran_Marine && who->getUpgradeLevel(UpgradeTypes::U_238_Shells)) || (unitType == UnitTypes::Zerg_Hydralisk && who->getUpgradeLevel(UpgradeTypes::Grooved_Spines)))
	{
		return 160.0;
	}
	else if (unitType == UnitTypes::Terran_Bunker)
	{
		if (who->getUpgradeLevel(UpgradeTypes::U_238_Shells))
		{
			return 192.0;
		}
		else
		{
			return 160.0;
		}
	}
	else if (unitType == UnitTypes::Protoss_High_Templar)
	{
		return 288.0;
	}

	// Correct range of Reavers
	else if (unitType == UnitTypes::Protoss_Reaver)
	{
		return 256.0;
	}
	return double(unitType.groundWeapon().maxRange());
}

double UtilTrackerClass::getTrueAirRange(UnitType unitType, Player who)
{
	if (unitType == UnitTypes::Protoss_Dragoon && who->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
	{
		return 192.0;
	}
	else if ((unitType == UnitTypes::Terran_Marine && who->getUpgradeLevel(UpgradeTypes::U_238_Shells)) || (unitType == UnitTypes::Zerg_Hydralisk && who->getUpgradeLevel(UpgradeTypes::Grooved_Spines)))
	{
		return 160.0;
	}
	else if (unitType == UnitTypes::Terran_Bunker)
	{
		if (who->getUpgradeLevel(UpgradeTypes::U_238_Shells))
		{
			return 192.0;
		}
		else
		{
			return 160.0;
		}
	}
	else if (unitType == UnitTypes::Terran_Goliath && who->getUpgradeLevel(UpgradeTypes::Charon_Boosters))
	{
		return 256.0;
	}
	else if (unitType == UnitTypes::Protoss_High_Templar)
	{
		return 288.0;
	}
	return double(unitType.airWeapon().maxRange());
}

double UtilTrackerClass::getTrueGroundDamage(UnitType unitType, Player who)
{
	if (unitType == UnitTypes::Protoss_Reaver)
	{
		if (who->getUpgradeLevel(UpgradeTypes::Scarab_Damage))
		{
			return 125.00;
		}
		else
		{
			return 100.00;
		}
	}
	else if (unitType == UnitTypes::Terran_Bunker)
	{
		return 25.0;
	}
	else if (unitType == UnitTypes::Terran_Firebat || unitType == UnitTypes::Protoss_Zealot)
	{
		return 16.0;
	}
	else
	{
		return unitType.groundWeapon().damageAmount();
	}
}

double UtilTrackerClass::getTrueAirDamage(UnitType unitType, Player who)
{
	if (unitType == UnitTypes::Terran_Bunker)
	{
		return 25.0;
	}
	return unitType.airWeapon().damageAmount();
}

double UtilTrackerClass::getTrueSpeed(UnitType unitType, Player who)
{
	double speed = unitType.topSpeed() * 32.0;

	if ((unitType == UnitTypes::Zerg_Zergling && who->getUpgradeLevel(UpgradeTypes::Metabolic_Boost)) || (unitType == UnitTypes::Zerg_Hydralisk && who->getUpgradeLevel(UpgradeTypes::Muscular_Augments)) || (unitType == UnitTypes::Zerg_Ultralisk && who->getUpgradeLevel(UpgradeTypes::Anabolic_Synthesis)) || (unitType == UnitTypes::Protoss_Shuttle && who->getUpgradeLevel(UpgradeTypes::Gravitic_Drive)) || (unitType == UnitTypes::Protoss_Observer && who->getUpgradeLevel(UpgradeTypes::Gravitic_Boosters)) || (unitType == UnitTypes::Protoss_Zealot && who->getUpgradeLevel(UpgradeTypes::Leg_Enhancements)) || (unitType == UnitTypes::Terran_Vulture && who->getUpgradeLevel(UpgradeTypes::Ion_Thrusters)))
	{
		speed = speed * 1.5;
	}
	else if (unitType == UnitTypes::Zerg_Overlord && who->getUpgradeLevel(UpgradeTypes::Pneumatized_Carapace))
	{
		speed = speed * 4.01;
	}
	else if (unitType == UnitTypes::Protoss_Scout && who->getUpgradeLevel(UpgradeTypes::Muscular_Augments))
	{
		speed = speed * 1.33;
	}
	return speed;
}

int UtilTrackerClass::getMinStopFrame(UnitType unitType)
{
	if (unitType == UnitTypes::Protoss_Dragoon)
	{
		return 9;
	}
	return 0;
}

WalkPosition UtilTrackerClass::getWalkPosition(Unit unit)
{
	int x = unit->getPosition().x;
	int y = unit->getPosition().y;

	// If it's a unit, we want to find the closest mini tile with the highest resolution (actual pixel width/height)
	if (!unit->getType().isBuilding())
	{
		int mini_x = int((x - (0.5*unit->getType().width())) / 8);
		int mini_y = int((y - (0.5*unit->getType().height())) / 8);
		return WalkPosition(mini_x, mini_y);
	}
	// For buildings, we want the actual tile size resolution (convert half the tile size to pixels by 0.5*tile*32 = 16.0)
	else
	{
		int mini_x = int((x - (16.0*unit->getType().tileWidth())) / 8);
		int mini_y = int((y - (16.0*unit->getType().tileHeight())) / 8);
		return WalkPosition(mini_x, mini_y);
	}
	return WalkPositions::None;
}

set<WalkPosition> UtilTrackerClass::getWalkPositionsUnderUnit(Unit unit)
{
	WalkPosition start = Units().getMyUnits()[unit].getWalkPosition();
	set<WalkPosition> returnValues;

	for (int i = start.x; i <= start.x + unit->getType().tileWidth(); i++)
	{
		for (int j = start.y; j <= start.y + unit->getType().tileHeight(); j++)
		{
			returnValues.emplace(WalkPosition(i, j));
		}
	}
	return returnValues;
}

bool UtilTrackerClass::isSafe(WalkPosition start, WalkPosition end, UnitType unitType, bool groundCheck, bool airCheck, bool mobilityCheck)
{
	for (int i = end.x - (unitType.width() / 16); i < end.x + (unitType.width() / 16); i++)
	{
		for (int j = end.y - (unitType.height() / 16); j < end.y + (unitType.height() / 16); j++)
		{
			if (WalkPosition(i, j).isValid())
			{
				// If mini tile exists on top of unit, ignore it
				if (i >= start.x && i < start.x + (unitType.tileWidth() * 4) && j >= start.y && j < start.y + (unitType.tileHeight() * 4))
				{
					continue;
				}
				if ((groundCheck && Grids().getEGroundDistanceGrid(i, j) != 0.0) || (airCheck && Grids().getEAirDistanceGrid(i, j) != 0.0) || (mobilityCheck && (Grids().getMobilityGrid(i, j) == 0 || Grids().getAntiMobilityGrid(i, j) == 1)))
				{
					return false;
				}
			}
		}
	}
	return true;
}