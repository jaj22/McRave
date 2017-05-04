#include "UnitInfo.h"

double unitGetStrength(UnitType unitType)
{
	// Some hardcoded values
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
	if (unitType == UnitTypes::Protoss_Arbiter || unitType == UnitTypes::Terran_Science_Vessel)
	{
		return 100.0;
	}
	if (unitType == UnitTypes::Protoss_Reaver)
	{
		return 50.0;
	}
	if (unitType == UnitTypes::Protoss_High_Templar)
	{
		return 20.0;
	}
	if (unitType == UnitTypes::Protoss_Scarab)
	{
		return 0.0;
	}

	if (unitType == UnitTypes::Zerg_Egg || unitType == UnitTypes::Zerg_Larva)
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
		if (unitType == UnitTypes::Protoss_Zealot /*|| unitType == UnitTypes::Terran_Firebat*/)
		{
			damage = damage * 2.0;
		}

		// Check for Zergling attack speed upgrade
		if (unitType == UnitTypes::Zerg_Zergling && Broodwar->enemy()->getUpgradeLevel(UpgradeTypes::Adrenal_Glands))
		{
			damage = damage * 1.33;
		}

		// Check for movement speed upgrades


		// Hp		
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
	return 0.5;
}
double unitGetAirStrength(UnitType unitType)
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
double unitGetVisibleStrength(Unit unit)
{
	if (unit->isMaelstrommed() || unit->isStasised())
	{
		return 0;
	}	

	double hp = double(unit->getHitPoints() + (unit->getShields() / 2)) / double(unit->getType().maxHitPoints() + (unit->getType().maxShields() / 2));
	if (unit->isStimmed())
	{
		
	}
	if (unit->getPlayer() == Broodwar->self() && unit->isCloaked() && !unit->isDetected())
	{
		return 4.0 * hp * unitGetStrength(unit->getType());
	}
	return hp * unitGetStrength(unit->getType());
}
double unitGetTrueRange(UnitType unitType, Player who)
{
	// Ranged upgrade check
	if (unitType == UnitTypes::Protoss_Dragoon && who->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
	{
		return 192.0;
	}
	else if ((unitType == UnitTypes::Terran_Marine && who->getUpgradeLevel(UpgradeTypes::U_238_Shells)) || (unitType == UnitTypes::Zerg_Hydralisk && who->getUpgradeLevel(UpgradeTypes::Grooved_Spines)))
	{
		return 160.0;
	}
	else if (unitType == UnitTypes::Protoss_Reaver)
	{
		return 256.0;
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
	return double(unitType.groundWeapon().maxRange());
}

void storeEnemyUnit(Unit unit, map<Unit, UnitInfoClass>& enemyUnits)
{
	// Create new unit
	if (enemyUnits.find(unit) == enemyUnits.end())
	{
		UnitInfoClass newUnit(unit->getType(), unit->getPosition(), unitGetVisibleStrength(unit), unitGetStrength(unit->getType()), unitGetTrueRange(unit->getType(), Broodwar->enemy()), unit->getLastCommand().getType(), 0, 0, 0);
		enemyUnits[unit] = newUnit;
	}
	// Update unit
	else
	{
		enemyUnits[unit].setUnitType(unit->getType());
		enemyUnits[unit].setPosition(unit->getPosition());
		enemyUnits[unit].setStrength(unitGetVisibleStrength(unit));
		enemyUnits[unit].setStrength(unitGetStrength(unit->getType()));
		enemyUnits[unit].setRange(unitGetTrueRange(unit->getType(), Broodwar->enemy()));
	}

	if ((unit->isCloaked() || unit->isBurrowed()) && !unit->isDetected())
	{
		enemyUnits[unit].setStrength(unitGetStrength(unit->getType()));
	}
	return;
}
void storeAllyUnit(Unit unit, map<Unit, UnitInfoClass>& allyUnits)
{
	// Create new unit
	if (allyUnits.find(unit) == allyUnits.end())
	{
		UnitInfoClass newUnit(unit->getType(), unit->getPosition(), unitGetVisibleStrength(unit), unitGetStrength(unit->getType()), unitGetTrueRange(unit->getType(), Broodwar->enemy()), unit->getLastCommand().getType(), 0, 0, 0);
		allyUnits[unit] = newUnit;
	}
	// Update unit
	else
	{
		allyUnits[unit].setUnitType(unit->getType());
		allyUnits[unit].setPosition(unit->getPosition());
		allyUnits[unit].setStrength(unitGetVisibleStrength(unit));
		allyUnits[unit].setRange(unitGetTrueRange(unit->getType(), Broodwar->self()));
		allyUnits[unit].setCommand(unit->getLastCommand().getType());
	}
	return;
}

UnitInfoClass::UnitInfoClass(UnitType newType, Position newPosition, double newStrength, double newMaxStrength, double newRange, UnitCommandType newCommand, int newDeadFrame, int newStrategy, int newCommandFrame)
{
	unitType = newType;
	unitPosition = newPosition;
	unitStrength = newStrength;
	unitMaxStrength = newMaxStrength;
	unitRange = newRange;
	unitCommand = newCommand;
	deadFrame = newDeadFrame;
	strategy = newStrategy;
	lastCommandFrame = newCommandFrame;
}
UnitInfoClass::UnitInfoClass()
{
	unitType = UnitTypes::Enum::None;
	unitPosition = Positions::None;
	unitStrength = 0.0;
	unitCommand = UnitCommandTypes::None;
}
UnitInfoClass::~UnitInfoClass()
{
}

Position UnitInfoClass::getPosition() const
{
	return unitPosition;
}
Position UnitInfoClass::getTargetPosition() const
{
	return targetPosition;
}
UnitType UnitInfoClass::getUnitType() const
{
	return unitType;
}
double UnitInfoClass::getStrength() const
{
	return unitStrength;
}
double UnitInfoClass::getMaxStrength() const
{
	return unitMaxStrength;
}
double UnitInfoClass::getRange() const
{
	return unitRange;
}
double UnitInfoClass::getLocal() const
{
	return unitLocal;
}
UnitCommandType UnitInfoClass::getCommand() const
{
	return unitCommand;
}
Unit UnitInfoClass::getTarget() const
{
	return target;
}
int UnitInfoClass::getDeadFrame() const
{
	return deadFrame;
}
int UnitInfoClass::getStrategy() const
{
	return strategy;
}
int UnitInfoClass::getLastCommandFrame() const
{
	return lastCommandFrame;
}

void UnitInfoClass::setUnitType(UnitType newUnitType)
{
	unitType = newUnitType;
}
void UnitInfoClass::setPosition(Position newPosition)
{
	unitPosition = newPosition;
}
void UnitInfoClass::setTargetPosition(Position newTargetPosition)
{
	targetPosition = newTargetPosition;
}
void UnitInfoClass::setStrength(double newStrength)
{
	unitStrength = newStrength;
}
void UnitInfoClass::setMaxStrength(double newMaxStrength)
{
	unitMaxStrength = newMaxStrength;
}
void UnitInfoClass::setLocal(double newUnitLocal)
{
	unitLocal = newUnitLocal;
}
void UnitInfoClass::setRange(double newRange)
{
	unitRange = newRange;
}
void UnitInfoClass::setCommand(UnitCommandType newCommand)
{
	unitCommand = newCommand;
}
void UnitInfoClass::setTarget(Unit newTarget)
{
	target = newTarget;
}
void UnitInfoClass::setDeadFrame(int newDeadFrame)
{
	deadFrame = newDeadFrame;
}
void UnitInfoClass::setStrategy(int newStrategy)
{
	strategy = newStrategy;
}
void UnitInfoClass::setLastCommandFrame(int newCommandFrame)
{
	lastCommandFrame = newCommandFrame;
}
