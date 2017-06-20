#include "McRave.h"

double UnitUtilClass::getStrength(UnitType unitType)
{
	// Some hardcoded values that don't have attacks but should still be considered for strength
	if (unitType == UnitTypes::Terran_Bunker)
	{
		return 50.0;
	}
	if (unitType == UnitTypes::Terran_Medic)
	{
		return 5.0;
	}
	if (unitType == UnitTypes::Zerg_Lurker)
	{
		return 20.0;
	}
	if (unitType == UnitTypes::Protoss_Reaver || unitType == UnitTypes::Protoss_Carrier)
	{
		return 50.0;
	}
	if (unitType == UnitTypes::Protoss_High_Templar)
	{
		return 20.0;
	}
	if (unitType == UnitTypes::Protoss_Scarab || unitType == UnitTypes::Terran_Vulture_Spider_Mine || unitType == UnitTypes::Zerg_Egg || unitType == UnitTypes::Zerg_Larva || unitType == UnitTypes::Protoss_Interceptor)
	{
		return 0.0;
	}

	if (!unitType.isWorker() && unitType != UnitTypes::Protoss_Scarab && unitType != UnitTypes::Terran_Vulture_Spider_Mine && unitType.groundWeapon().damageAmount() > 0 || (unitType.isBuilding() && unitType.groundWeapon().damageAmount() > 0))
	{
		double range, damage, hp, speed;
		// Range upgrade check (applies to enemy Dragoons, not a big issue currently)
		if (unitType == UnitTypes::Protoss_Dragoon && Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
		{
			range = 192.0;
		}

		// Enemy ranged upgrade check
		else if ((unitType == UnitTypes::Terran_Marine && Broodwar->enemy()->getUpgradeLevel(UpgradeTypes::U_238_Shells)) || (unitType == UnitTypes::Zerg_Hydralisk && Broodwar->enemy()->getUpgradeLevel(UpgradeTypes::Grooved_Spines)))
		{
			range = 160.0;
		}
		else
		{
			range = double(unitType.groundWeapon().maxRange());
		}

		// Damage
		damage = double(unitType.groundWeapon().damageAmount()) / double(unitType.groundWeapon().damageCooldown());

		// Speed
		speed = 1.0 + double(unitType.topSpeed());

		// Zealot and Firebat has to be doubled for two attacks
		if (unitType == UnitTypes::Protoss_Zealot || unitType == UnitTypes::Terran_Firebat)
		{
			damage = damage * 2.0;
		}

		// Check for Zergling attack speed upgrade
		if (unitType == UnitTypes::Zerg_Zergling && Broodwar->enemy()->getUpgradeLevel(UpgradeTypes::Adrenal_Glands))
		{
			damage = damage * 1.33;
		}

		// Hp, assume half strength for shields, they're not very strong in most matchups
		hp = double(unitType.maxHitPoints() + (unitType.maxShields() / 2));

		// Assume strength doubled for units that can use Stim to prevent poking into armies frequently
		if (stimResearched && (unitType == UnitTypes::Terran_Marine || unitType == UnitTypes::Terran_Firebat))
		{
			return 20.0 * (1.0 + (range / 320.0)) * damage * (hp / 100.0);
		}

		return 10.0 * (1.0 + (range / 320.0)) * damage * (hp / 100.0);
	}
	if (unitType.isWorker())
	{
		return 5.0;
	}
	return 1.0;
}

double UnitUtilClass::getAirStrength(UnitType unitType)
{
	double range, damage, hp;
	range = double(unitType.airWeapon().maxRange());

	// Enemy ranged upgrade check
	if (unitType == UnitTypes::Terran_Goliath && Broodwar->enemy()->getUpgradeLevel(UpgradeTypes::Charon_Boosters))
	{
		range = 256.0;
	}
	else if (unitType == UnitTypes::Protoss_Dragoon && Broodwar->enemy()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
	{
		range = 192.0;
	}
	else if ((unitType == UnitTypes::Terran_Marine && Broodwar->enemy()->getUpgradeLevel(UpgradeTypes::U_238_Shells)) || (unitType == UnitTypes::Zerg_Hydralisk && Broodwar->enemy()->getUpgradeLevel(UpgradeTypes::Grooved_Spines)))
	{
		range = 160.0;
	}
	else
	{
		range = double(unitType.airWeapon().maxRange());
	}

	// Damage
	damage = double(unitType.airWeapon().damageAmount()) / double(unitType.airWeapon().damageCooldown());

	// Hp		
	hp = double(unitType.maxHitPoints() + (unitType.maxShields() / 2));

	return sqrt(1 + (range / 320.0)) * damage * (hp / 100);
}

double UnitUtilClass::getVisibleStrength(Unit unit)
{
	if (unit->isMaelstrommed() || unit->isStasised())
	{
		return 0;
	}

	double hp = double(unit->getHitPoints() + (unit->getShields() / 2)) / double(unit->getType().maxHitPoints() + (unit->getType().maxShields() / 2));
	if (unit->isStimmed())
	{
		stimResearched = true;
	}

	// Make units under an Arbiter feel stronger
	if (unit->getPlayer() == Broodwar->self() && unit->isCloaked() && !unit->isDetected())
	{
		return 4.0 * hp * getStrength(unit->getType());
	}
	return hp * getStrength(unit->getType());
}

double UnitUtilClass::getTrueRange(UnitType unitType, Player who)
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

	// Correct range of Reavers
	else if (unitType == UnitTypes::Protoss_Reaver)
	{
		return 256.0;
	}
	return double(unitType.groundWeapon().maxRange());
}

double UnitUtilClass::getTrueAirRange(UnitType unitType, Player who)
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
	return double(unitType.airWeapon().maxRange());
}

double UnitUtilClass::getTrueGroundDamage(UnitType unitType, Player who)
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
		return 25;
	}
	else
	{
		return unitType.groundWeapon().damageAmount();
	}
}

double UnitUtilClass::getTrueAirDamage(UnitType unitType, Player who)
{
	if (unitType == UnitTypes::Terran_Bunker)
	{
		return 25;
	}
	return unitType.airWeapon().damageAmount();
}

double UnitUtilClass::getPriority(UnitType unitType)
{
	// Low strength units with high threat	
	if (unitType == UnitTypes::Protoss_Arbiter || unitType == UnitTypes::Protoss_Observer || unitType == UnitTypes::Protoss_Shuttle || unitType == UnitTypes::Terran_Science_Vessel || unitType == UnitTypes::Terran_Dropship || unitType == UnitTypes::Terran_Vulture_Spider_Mine)
	{
		return 50.0;
	}
	// Reduce the value of a bunker so units don't only target bunkers, since they can be repaired easily
	else if (unitType == UnitTypes::Terran_Bunker)
	{
		return 2.5;
	}
	// Reduce the value of carriers so interceptors are killed instead
	else if (unitType == UnitTypes::Protoss_Carrier)
	{
		return 2.5;
	}
	else
	{
		return getStrength(unitType);
	}
}

double UnitUtilClass::getTrueSpeed(UnitType unitType, Player who)
{
	double speed = unitType.topSpeed();

	// Adjust based on speed upgrades for Zerglings, Hydralisks, Ultralisks, Shuttle, Observer, Zealot, Vulture of 50%
	if ((unitType == UnitTypes::Zerg_Zergling && who->getUpgradeLevel(UpgradeTypes::Metabolic_Boost)) || (unitType == UnitTypes::Zerg_Hydralisk && who->getUpgradeLevel(UpgradeTypes::Muscular_Augments)) || (unitType == UnitTypes::Zerg_Ultralisk && who->getUpgradeLevel(UpgradeTypes::Anabolic_Synthesis)) || (unitType == UnitTypes::Protoss_Shuttle && who->getUpgradeLevel(UpgradeTypes::Gravitic_Drive)) || (unitType == UnitTypes::Protoss_Observer && who->getUpgradeLevel(UpgradeTypes::Gravitic_Boosters)) || (unitType == UnitTypes::Protoss_Zealot && who->getUpgradeLevel(UpgradeTypes::Leg_Enhancements)) || (unitType == UnitTypes::Terran_Vulture && who->getUpgradeLevel(UpgradeTypes::Ion_Thrusters)))
	{
		speed = speed * 1.5;
	}
	else if (unitType == UnitTypes::Zerg_Overlord && who->getUpgradeLevel(UpgradeTypes::Pneumatized_Carapace))
	{
		speed = speed * 4.0;
	}
	else if (unitType == UnitTypes::Protoss_Scout && who->getUpgradeLevel(UpgradeTypes::Muscular_Augments))
	{
		speed = speed * 1.33;
	}
	return speed;
}

WalkPosition UnitUtilClass::getMiniTile(Unit unit)
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

set<WalkPosition> UnitUtilClass::getMiniTilesUnderUnit(Unit unit)
{
	WalkPosition start = Units().getMyUnits()[unit].getMiniTile();		
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