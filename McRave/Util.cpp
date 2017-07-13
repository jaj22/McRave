#include "McRave.h"

double UtilTrackerClass::getMaxGroundStrength(UnitInfo& unit, Player who)
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
	if (unit.getType() == UnitTypes::Protoss_Arbiter)
	{
		return 50.0;
	}
	if (unit.getType() == UnitTypes::Protoss_Scarab || unit.getType() == UnitTypes::Terran_Vulture_Spider_Mine || unit.getType() == UnitTypes::Zerg_Egg || unit.getType() == UnitTypes::Zerg_Larva || unit.getType() == UnitTypes::Protoss_Interceptor)
	{
		return 0.0;
	}

	double range, damage, speed;
	range = cbrt(unit.getGroundRange());
	if (unit.getType().groundWeapon().damageCooldown() > 0)
	{
		damage = unit.getGroundDamage() / double(unit.getType().groundWeapon().damageCooldown());
	}
	else
	{
		damage = unit.getGroundDamage();
	}
	speed = unit.getSpeed()/128.0;

	if (!unit.getType().isWorker() && unit.getGroundDamage() > 0)
	{
		// Check for Zergling attack speed upgrade
		if (unit.getType() == UnitTypes::Zerg_Zergling && who->getUpgradeLevel(UpgradeTypes::Adrenal_Glands))
		{
			damage = damage * 1.33;
		}
		if (speed > 0)
		{
			return range * damage * speed ;
		}
		else
		{
			return range * damage;
		}
	}
	return 0.0;
}

double UtilTrackerClass::getVisibleGroundStrength(UnitInfo& unit, Player who)
{
	if (unit.unit()->isMaelstrommed() || unit.unit()->isStasised())
	{
		return 0;
	}

	double effectiveness = 1.0;
	double hp = double(unit.unit()->getHitPoints() + (unit.unit()->getShields() / 2)) / double(unit.getType().maxHitPoints() + (unit.getType().maxShields() / 2));

	/*int aLarge = Units().getMySizes()[UnitSizeTypes::Large];
	int aMedium = Units().getMySizes()[UnitSizeTypes::Medium];
	int aSmall = Units().getMySizes()[UnitSizeTypes::Small];

	int eLarge = Units().getEnSizes()[UnitSizeTypes::Large];
	int eMedium = Units().getEnSizes()[UnitSizeTypes::Medium];
	int eSmall = Units().getEnSizes()[UnitSizeTypes::Small];


	if (unit.getType().groundWeapon().damageType() == DamageTypes::Explosive)
	{
		effectiveness = double((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / double(eLarge + eMedium + eSmall);
	}*/

	if ((unit.unit()->isCloaked() || unit.unit()->isBurrowed()) && !unit.unit()->isDetected())
	{
		return 10.0 * unit.getMaxGroundStrength() * effectiveness;
	}
	return hp * unit.getMaxGroundStrength() * effectiveness;
}

double UtilTrackerClass::getMaxAirStrength(UnitInfo& unit, Player who)
{
	double range, damage, speed;
	range = cbrt(unit.getAirRange());
	damage = unit.getAirDamage() / double(unit.getType().airWeapon().damageCooldown());
	speed = unit.getSpeed()/128.0;

	if (!unit.getType().isWorker() && damage > 0)
	{
		if (speed > 0)
		{
			return range * damage * speed;
		}
		else
		{
			return range * damage;
		}
	}
	return 0.0;
}

double UtilTrackerClass::getVisibleAirStrength(UnitInfo& unit, Player who)
{
	if (unit.unit()->isMaelstrommed() || unit.unit()->isStasised())
	{
		return 0;
	}

	double hp = double(unit.unit()->getHitPoints() + (unit.unit()->getShields())) / double(unit.getType().maxHitPoints() + (unit.getType().maxShields()));


	if ((unit.unit()->isCloaked() || unit.unit()->isBurrowed()) && !unit.unit()->isDetected())
	{
		return 10.0 * unit.getMaxAirStrength();
	}
	return hp * unit.getMaxAirStrength();
}

double UtilTrackerClass::getPriority(UnitInfo& unit, Player who)
{
	// If an enemy detector is within range of an Arbiter, give it higher priority
	if (Grids().getArbiterGrid(unit.getWalkPosition()) > 0 && unit.getType().isDetector() && unit.getPlayer()->isEnemy(Broodwar->self()))
	{
		return 10.0;
	}

	// Support units gain higher priority due to their capabilities
	if (unit.getType() == UnitTypes::Protoss_Arbiter || unit.getType() == UnitTypes::Protoss_Observer || unit.getType() == UnitTypes::Protoss_Shuttle || unit.getType() == UnitTypes::Terran_Science_Vessel || unit.getType() == UnitTypes::Terran_Dropship || unit.getType() == UnitTypes::Terran_Vulture_Spider_Mine)
	{
		return 50.0;
	}

	// Workers get a fairly low priority
	else if (unit.getType().isWorker())
	{
		return 1.0;
	}

	// Buildings with no attack have the lowest priority
	else if (unit.getType().isBuilding() && unit.getMaxGroundStrength() == 0 && unit.getMaxAirStrength() == 0)
	{
		return 0.5;
	}

	// Overlords have low priority but are worthwhile to pick off
	else if (unit.getType() == UnitTypes::Zerg_Overlord)
	{
		return 2.0;
	}

	// Else return the units max strength
	else
	{
		return max(unit.getMaxGroundStrength(), unit.getMaxAirStrength());
	}
}

double UtilTrackerClass::getTrueRange(UnitType unitType, Player who)
{
	// Range upgrade check for Dragoons, Marines and Hydralisks ground attack
	if (unitType == UnitTypes::Protoss_Dragoon && who->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
	{
		return 192.0;
	}
	else if ((unitType == UnitTypes::Terran_Marine && who->getUpgradeLevel(UpgradeTypes::U_238_Shells)) || (unitType == UnitTypes::Zerg_Hydralisk && who->getUpgradeLevel(UpgradeTypes::Grooved_Spines)))
	{
		return 160.0;
	}

	// Range upgrade check and correction of initial range for Bunkers ground attack
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

	// Range assumption for High Templars
	else if (unitType == UnitTypes::Protoss_High_Templar)
	{
		return 288.0;
	}

	// Correction of initial range for Reavers 
	else if (unitType == UnitTypes::Protoss_Reaver)
	{
		return 256.0;
	}
	return double(unitType.groundWeapon().maxRange());
}

double UtilTrackerClass::getTrueAirRange(UnitType unitType, Player who)
{
	// Range upgrade check for Dragoons, Marines and Hydralisks air attack
	if (unitType == UnitTypes::Protoss_Dragoon && who->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
	{
		return 192.0;
	}
	else if ((unitType == UnitTypes::Terran_Marine && who->getUpgradeLevel(UpgradeTypes::U_238_Shells)) || (unitType == UnitTypes::Zerg_Hydralisk && who->getUpgradeLevel(UpgradeTypes::Grooved_Spines)))
	{
		return 160.0;
	}

	// Range upgrade check and correction of initial range for Bunkers air attack
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

	// Range upgrade check for Goliaths air attack
	else if (unitType == UnitTypes::Terran_Goliath && who->getUpgradeLevel(UpgradeTypes::Charon_Boosters))
	{
		return 256.0;
	}

	// Range assumption for High Templars
	else if (unitType == UnitTypes::Protoss_High_Templar)
	{
		return 288.0;
	}

	// Else return the Units base range for air weapons
	return double(unitType.airWeapon().maxRange());
}

double UtilTrackerClass::getTrueGroundDamage(UnitType unitType, Player who)
{
	// Damage upgrade check for Reavers and correction of initial damage
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

	// Damage assumption for Bunkers ground attack
	else if (unitType == UnitTypes::Terran_Bunker)
	{
		return 25.0;
	}

	// Damage correction for Zealots and Firebats which attack twice for 8 damage
	else if (unitType == UnitTypes::Terran_Firebat || unitType == UnitTypes::Protoss_Zealot)
	{
		return 16.0;
	}

	// Else return the Units base ground weapon damage
	else
	{
		return unitType.groundWeapon().damageAmount();
	}

	return 0.0;
}

double UtilTrackerClass::getTrueAirDamage(UnitType unitType, Player who)
{
	// Damage assumption for Bunkers air attack
	if (unitType == UnitTypes::Terran_Bunker)
	{
		return 25.0;
	}
	
	// Else return the Units base air weapon damage
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
	int pixelX = unit->getPosition().x;
	int pixelY = unit->getPosition().y;

	// If it's a unit, we want to find the closest mini tile with the highest resolution (actual pixel width/height)
	if (!unit->getType().isBuilding())
	{
		int walkX = int((pixelX - (0.5*unit->getType().width())) / 8);
		int walkY = int((pixelY - (0.5*unit->getType().height())) / 8);
		return WalkPosition(walkX, walkY);
	}
	// For buildings, we want the actual tile size resolution (convert half the tile size to pixels by 0.5*tile*32 = 16.0)
	else
	{
		int walkX = int((pixelX - (16.0*unit->getType().tileWidth())) / 8);
		int walkY = int((pixelY - (16.0*unit->getType().tileHeight())) / 8);
		return WalkPosition(walkX, walkY);
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
	for (int i = end.x - (unitType.tileWidth() * 2); i < end.x + (unitType.tileWidth() * 2); i++)
	{
		for (int j = end.y - (unitType.tileHeight() * 2); j < end.y + (unitType.tileHeight() * 2); j++)
		{
			if (WalkPosition(i, j).isValid())
			{				
				// If WalkPosition shared with WalkPositions under unit, ignore
				if (i >= start.x && i <= (start.x + (unitType.tileWidth() * 2)) && j >= start.y && j <= (start.y + (unitType.tileHeight() * 2)))
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