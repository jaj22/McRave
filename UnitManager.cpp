#include "UnitManager.h"
#include "TargetManager.h"
#include "BWTA.h"

using namespace BWAPI;
using namespace std;
using namespace BWTA;

bool harassing = false;
bool stimResearched = false;
int radius = 1500;

// Map invisible units to number of detectors moving to position?
// To check: Crash issues, build order issues

#pragma region Strategy

void unitMicro(Unit unit, Unit target)
{
	// Variables
	bool kite;
	int range = unit->getType().groundWeapon().maxRange();

	// Range check	
	if (unit->getType() == UnitTypes::Protoss_Dragoon && Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
	{
		range = 192;
	}
	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		range = 256;
	}

	// Store current target position
	unitsCurrentTarget[unit] = target->getPosition();

	// If unit is attacking or in an attack frame, don't want to micro
	if (unit->isAttackFrame() || unit->isStartingAttack())
	{
		return;
	}

	// If recently issued a command to attack a unit, don't want to micro
	if (unit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit->getTarget() == target)
	{
		return;
	}

	// If kiting unnecessary, disable
	if (target->getType().isFlyer() || target->getType().isBuilding() || unit->getType() == UnitTypes::Protoss_Corsair)
	{
		kite = false;
	}

	// If kiting is a good idea, enable
	else if (target->getType() == UnitTypes::Terran_Vulture_Spider_Mine || (range > 32 && unit->isUnderAttack()) || (target->getType().groundWeapon().maxRange() <= range && (unit->getDistance(target) < range - target->getType().groundWeapon().maxRange() && target->getType().groundWeapon().maxRange() > 0 || unit->getHitPoints() < 40)))
	{
		kite = true;
	}

	// If kite is true and weapon on cooldown, move
	if (kite && Broodwar->getLatencyFrames() / 2 <= unit->getGroundWeaponCooldown())
	{
		Position correctedFleePosition = unitFlee(unit, target);
		// Want Corsairs to move closer always if possible
		if (unit->getType() == UnitTypes::Protoss_Corsair)
		{
			unit->move(target->getPosition());
			unitsCurrentTarget[unit] = target->getPosition();
		}
		else if (correctedFleePosition != BWAPI::Positions::None)
		{
			unit->move(Position(correctedFleePosition.x + rand() % 2 + (-1), correctedFleePosition.y + rand() % 2 + (-1)));
			unitsCurrentTarget[unit] = correctedFleePosition;
		}
	}
	// Else, regardless of if kite is true or not, attack if weapon is off cooldown
	else if (Broodwar->getLatencyFrames() / 2 > unit->getGroundWeaponCooldown())
	{
		unit->attack(target);
	}
	return;
}

int unitGetGlobalStrategy()
{
	/*if (Broodwar->self()->supplyUsed() > 300)
	{
	return 1;
	}*/
	if ((Broodwar->enemy()->getRace() == Races::Terran || Broodwar->enemy()->getRace() == Races::Protoss) && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) > 0)
	{
		return 1;
	}
	if (allyStrength > enemyStrength && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int unitGetLocalStrategy(Unit unit, Unit target)
{
	/* Check for local strength, based on that make an adjustment
	Return 0 = retreat to holding position
	Return 1 = fight enemy
	Return 2 = disregard local calculations
	Return 3 = disregard local */

	double mod = 0.0, thisUnit = 0.0;
	double enemyLocalStrength = 0.0, allyLocalStrength = 0.0;

	if (unit->getType() == UnitTypes::Protoss_Zealot && (target->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || target->getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && unit->getDistance(target) < 64)
	{
		return 1;
	}

	for (auto enemy : enemyUnits)
	{
		thisUnit = 0.0;
		Unit u = Broodwar->getUnit(enemy.first);

		// If a unit is within range of the target, add to local strength
		if (enemy.second.getPosition().getDistance(target->getPosition()) < enemy.second.getUnitType().groundWeapon().maxRange() || (enemy.second.getUnitType().groundWeapon().maxRange() < 64 && enemy.second.getPosition().getDistance(target->getPosition()) < 256))
		{
			// If unit is visible, get visible strength, else estimate strength
			if (u && u->exists())
			{
				// If unit is cloaked or burrowed and not detected, drastically increase strength
				if ((u->isCloaked() || u->isBurrowed()) && !u->isDetected())
				{
					thisUnit = 20 * unitGetStrength(u->getType());
				}
				else if (u->getType().groundWeapon().damageType() == DamageTypes::Explosive)
				{
					thisUnit = unitGetVisibleStrength(u) * ((((double)aLarge * 1) + ((double)aMedium * 0.75) + ((double)aSmall * 0.5)) / double(1 + aLarge + aMedium + aSmall));
				}
				else if (u->getType().groundWeapon().damageType() == DamageTypes::Concussive)
				{
					thisUnit = unitGetVisibleStrength(u) * ((((double)aLarge * 0.25) + ((double)aMedium * 0.5) + ((double)aSmall * 1)) / double(1 + aLarge + aMedium + aSmall));
				}
				else
				{
					thisUnit = unitGetVisibleStrength(u);
				}
			}
			// Else used stored information
			else if (enemy.second.getUnitType().groundWeapon().damageType() == DamageTypes::Explosive)
			{
				thisUnit = unitGetStrength(enemy.second.getUnitType()) * ((((double)aLarge * 1) + ((double)aMedium * 0.75) + ((double)aSmall * 0.5)) / double(1 + aLarge + aMedium + aSmall));
			}
			else if (enemy.second.getUnitType().groundWeapon().damageType() == DamageTypes::Concussive)
			{
				thisUnit = unitGetStrength(enemy.second.getUnitType()) * ((((double)aLarge * 0.25) + ((double)aMedium * 0.5) + ((double)aSmall * 1)) / double(1 + aLarge + aMedium + aSmall));
			}
			else
			{
				thisUnit = unitGetStrength(enemy.second.getUnitType());
			}
		}
		enemyLocalStrength += thisUnit;
	}

	for (Unit ally : unit->getUnitsInRadius(radius, Filter::IsAlly && !Filter::IsBuilding && !Filter::IsWorker))
	{
		// If shuttle, add units inside
		if (ally->getType() == UnitTypes::Protoss_Shuttle && ally->getLoadedUnits().size() > 0)
		{
			// Assume reaver for damage type calculations
			for (Unit uL : ally->getLoadedUnits())
			{
				allyLocalStrength += unitGetVisibleStrength(uL);
			}
		}
		else
		{
			if (eLarge > 0 || eMedium > 0 || eSmall > 0)
			{
				// Damage type calculations
				if (ally->getType().groundWeapon().damageType() == DamageTypes::Explosive)
				{
					allyLocalStrength += unitGetVisibleStrength(ally) * ((((double)eLarge * 1.0) + ((double)eMedium * 0.75) + ((double)eSmall * 0.5)) / double(1 + eLarge + eMedium + eSmall));
				}
				else if (ally->getType().groundWeapon().damageType() == DamageTypes::Concussive)
				{
					allyLocalStrength += unitGetVisibleStrength(ally) * ((((double)eLarge * 0.25) + ((double)eMedium * 0.5) + ((double)eSmall * 1.0)) / double(1 + eLarge + eMedium + eSmall));
				}
				else
				{
					allyLocalStrength += unitGetVisibleStrength(ally);
				}
			}
			else
			{
				allyLocalStrength += unitGetVisibleStrength(ally);
			}
		}
	}
	// Include the unit itself into its calculations based on damage type
	if (eLarge > 0 || eMedium > 0 || eSmall > 0)
	{
		if (unit->getType().groundWeapon().damageType() == DamageTypes::Explosive)
		{
			allyLocalStrength += unitGetVisibleStrength(unit) * ((((double)eLarge * 1.0) + ((double)eMedium * 0.75) + ((double)eSmall * 0.5)) / double(1 + eLarge + eMedium + eSmall));
		}
		else if (unit->getType().groundWeapon().damageType() == DamageTypes::Concussive)
		{
			allyLocalStrength += unitGetVisibleStrength(unit) * ((((double)eLarge * 0.25) + ((double)eMedium * 0.5) + ((double)eSmall * 1.0)) / double(1 + eLarge + eMedium + eSmall));
		}
		else
		{
			allyLocalStrength += unitGetVisibleStrength(unit);
		}
	}
	else
	{
		allyLocalStrength += unitGetVisibleStrength(unit);
	}

	// If enemy local exists, update information for HUD
	if (enemyLocalStrength >= 0.0)
	{
		unitRadiusCheck[unit] = radius;
		localEnemy[unit] = enemyLocalStrength;
		localAlly[unit] = allyLocalStrength;
		//localEnemy[unit] = (localEnemy[unit] * 9 / 10) + (enemyLocalStrength / 10);
		//localAlly[unit] = (localAlly[unit] * 9 / 10) + (allyLocalStrength / 10);
	}
	// Else remove information if it exists		
	else
	{
		localEnemy.erase(unit);
		localAlly.erase(unit);
		unitRadiusCheck.erase(unit);
	}

	// If we are in ally territory and have a target, force to fight	
	if (target && target->exists())
	{
		if (find(allyTerritory.begin(), allyTerritory.end(), getRegion(target->getPosition())) != allyTerritory.end())
		{
			unitsCurrentLocalCommand[unit] = 1;
			return 1;
		}
	}

	// If we don't have a Cyber core yet, we don't want to push out of the base
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) < 1)
	{
		unitsCurrentLocalCommand[unit] = 2;
		return 2;
	}

	// If most recent local ally higher, return attack
	if (localAlly[unit] >= localEnemy[unit])
	{
		unitsCurrentLocalCommand[unit] = 1;
		return 1;
	}
	// Else return retreat
	else if (localAlly[unit] < localEnemy[unit])
	{
		unitsCurrentLocalCommand[unit] = 0;
		return 0;
	}
	// Disregard local if no target, no recent local calculation and not within ally region
	return 3;
}

void unitGetCommand(Unit unit)
{
	unitsCurrentTarget.erase(unit);
	double closestD = 0;
	Position closestP;
	Unit target;

	// Get target first
	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		target = getClusterTarget(unit);
	}
	else
	{
		target = getTarget(unit);
	}

	if (target && target->exists())
	{
		/* Check local strategy manager to see what our next task is.
		If 0, regroup unless forced to engage.
		If 1, send unit to micro-management. */
		int stratL = unitGetLocalStrategy(unit, target);		
		// If target and unit are both valid and we're not ignoring local calculations
		if (stratL != 3)
		{
			// Defend
			if (stratL == 2)
			{
				// If we are on top of our ramp, let's hold with zealots
				if (unit->getDistance(defendHere.at(0)) < 64 && unit->getType() == UnitTypes::Protoss_Zealot)
				{
					if (unit->getUnitsInRadius(64, Filter::IsEnemy).size() > 0)
					{
						unitMicro(unit, target);
						return;
					}
					else if (forceExpand == 0 && unit->getDistance(defendHere.at(0)) > 64)
					{
						unit->move(Position(defendHere.at(0).x + rand() % 3 + (-1), defendHere.at(0).y + rand() % 3 + (-1)));
						return;
					}
				}
			}
			// Attack
			if (stratL == 1 && target != unit)
			{
				unitMicro(unit, target);
				return;
			}
			// Retreat
			if (stratL == 0)
			{
				for (auto position : defendHere)
				{
					if (unit->getDistance(position) < 320 && find(allyTerritory.begin(), allyTerritory.end(), getRegion(unit->getTilePosition())) != allyTerritory.end())
					{
						Position fleePosition = unitFlee(unit, unit->getClosestUnit(Filter::IsEnemy));
						unit->move(Position(fleePosition.x + rand() % 3 + (-1), fleePosition.y + rand() % 3 + (-1)));
						return;
					}
					if (unit->getClosestUnit(Filter::IsEnemy))
					{
						if (unit->getClosestUnit(Filter::IsEnemy)->getDistance(position) < unit->getDistance(position) && unit->getDistance(playerStartingPosition) < unit->getDistance(enemyStartingPosition))
						{
							Position fleePosition = unitFlee(unit, unit->getClosestUnit(Filter::IsEnemy));
							unit->move(Position(fleePosition.x + rand() % 3 + (-1), fleePosition.y + rand() % 3 + (-1)));
							return;
						}
					}
					if (unit->getDistance(position) <= closestD || closestD == 0.0)
					{
						closestD = unit->getDistance(position);
						closestP = position;
					}
				}
				unit->move(Position(closestP.x + rand() % 3 + (-1), closestP.y + rand() % 3 + (-1)));
				return;
			}
		}
	}


	/* Check our global strategy manager to see what our next task is.
	If 0, regroup at a chokepoint connected to allied territory.
	If 1, send units into a frontal attack.	*/
	int stratG = unitGetGlobalStrategy();

	if (unitGetGlobalStrategy() == 0)
	{
		if (enemyBasePositions.size() > 0 && allyTerritory.size() > 0)
		{
			closestD = 1000.0;
			closestP = defendHere.at(0);
			// Check if we are close enough to any defensive position
			for (auto position : defendHere)
			{
				if (unit->getDistance(position) < 128)
				{
					closestP = position;
					break;
				}
				else if (unit->getDistance(position) <= closestD)
				{
					closestD = unit->getDistance(position);
					closestP = position;
				}
			}
			// If we forced to expand, move to next choke to prevent blocking 
			if (forceExpand == 1)
			{
				closestP = Position(path.at(1)->Center());
			}
			if (unit->getLastCommand().getTargetPosition() != closestP)
			{
				unit->move(Position(closestP.x + rand() % 3 + (-1), closestP.y + rand() % 3 + (-1)));
			}
		}
		else
		{
			// Else just defend at nearest chokepoint (starting of game without scout information)
			unit->move((getNearestChokepoint(unit->getPosition()))->getCenter());
			return;
		}
	}

	// Check if we should attack
	if (unitGetGlobalStrategy() == 1)
	{
		if (unit->getLastCommand().getType() != UnitCommandTypes::Attack_Move && enemyBasePositions.size() > 0)
		{
			unit->attack(enemyBasePositions.front());
			return;
		}
	}
}

#pragma endregion

#pragma region Strength

double unitGetStrength(UnitType unitType)
{
	// Some hardcoded values
	if (unitType == UnitTypes::Terran_Bunker)
	{
		return 5.0;
	}
	if (unitType == UnitTypes::Terran_Medic)
	{
		return 0.75;
	}
	if (unitType == UnitTypes::Zerg_Lurker)
	{
		return 2.0;
	}
	if (unitType == UnitTypes::Protoss_Arbiter || unitType == UnitTypes::Terran_Science_Vessel)
	{
		return 10.0;
	}
	if (unitType == UnitTypes::Protoss_Reaver)
	{
		return 5.0;
	}
	if (unitType == UnitTypes::Protoss_High_Templar)
	{
		return 2.0;
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

		speed = double(unitType.topSpeed());
		// Zealot and Firebat has to be doubled for two attacks
		if (unitType == UnitTypes::Protoss_Zealot || unitType == UnitTypes::Terran_Firebat)
		{
			damage = damage * 2;
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
			return 2 * ((1 + (range / 320.0)) * damage * (hp / 100));
		}

		return (1 + (range / 320.0)) * damage * (hp / 100);
	}
	return 0.0;
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

	// If a unit is visible, we want to get its current strength based on current health, shield and any buffs
	if (unit->getType().isWorker() && getRegion(unit->getTilePosition()) == getRegion(playerStartingTilePosition))
	{
		if (unit->getPlayer() == Broodwar->self() && find(combatProbe.begin(), combatProbe.end(), unit) != combatProbe.end())
		{
			return 0.1;
		}
		if (unit->getPlayer() == Broodwar->enemy())
		{
			return 0.1;
		}
		else
		{
			return 0.0;
		}
	}	

	double hp = double(unit->getHitPoints() + (unit->getShields() / 2)) / double(unit->getType().maxHitPoints() + (unit->getType().maxShields() / 2));
	if (unit->isStimmed())
	{
		stimResearched = true;
	}
	return hp * unitGetStrength(unit->getType());
}

#pragma endregion

#pragma region Targeting

Unit getTarget(Unit unit)
{
	double highest = 0.0, thisUnit = 0.0;
	Unit target = unit;

	// If we are being rushed, only focus the closest units to our starting position to fend them off
	if (enemyAggresion)
	{
		if (unit->getUnitsInRadius(320, Filter::IsEnemy && !Filter::IsFlyer).size() > 0)
		{
			return Broodwar->getClosestUnit(playerStartingPosition, Filter::IsEnemy && !Filter::IsFlyer);
		}
	}

	// If Zealot or Reaver
	if (unit->getType() == UnitTypes::Protoss_Zealot || unit->getType() == UnitTypes::Protoss_Reaver)
	{
		// Iterate through all enemies in a visible radius that are not air
		for (auto enemy : enemyUnits)
		{
			// If visible
			if (Broodwar->getUnit(enemy.first)->exists())
			{
				// Check if within radius
				if (enemy.second.getPosition().getDistance(unit->getPosition()) < radius && !enemy.second.getUnitType().isFlyer())
				{					
					if (enemy.second.getPosition().getDistance(unit->getPosition()) < 16)
					{
						thisUnit = 2.0 + unitGetStrength(enemy.second.getUnitType());
					}
					else if (enemy.second.getUnitType().isBuilding() && !enemy.second.getUnitType().canAttack())
					{
						thisUnit = 1.0 / (1.0 + double(unit->getDistance(enemy.second.getPosition())));
					}
					else if (enemy.second.getUnitType().isWorker())
					{
						// If it's an SCV that is repairing, make him priority
						if (unit->getType() == UnitTypes::Terran_SCV && unit->isRepairing())
						{
							thisUnit = 5.5;
						}
						else
						{
							thisUnit = 2.0 / (1.0 + double(unit->getDistance(enemy.second.getPosition())));
						}
					}
					else
					{
						thisUnit = 2.0 + (unitGetStrength(enemy.second.getUnitType()) / (1.0 + double(unit->getDistance(enemy.second.getPosition()))));
					}
					if (thisUnit > highest)
					{
						target = Broodwar->getUnit(enemy.first);
						highest = thisUnit;
					}
				}
			}
		}
	}
	// If Corsair
	else if (unit->getType() == UnitTypes::Protoss_Corsair)
	{
		// Iterate through all enemies in a visible radius that are air
		for (auto enemy : enemyUnits)
		{
			// If visible
			if (Broodwar->getUnit(enemy.first)->exists())
			{
				// Check if within radius
				if (enemy.second.getPosition().getDistance(unit->getPosition()) < radius && enemy.second.getUnitType().isFlyer())
				{
					thisUnit = 1.0 + (unitGetStrength(enemy.second.getUnitType()) / (1.0 + double(unit->getDistance(enemy.second.getPosition()))));
					if (thisUnit > highest)
					{
						target = Broodwar->getUnit(enemy.first);
						highest = thisUnit;
					}
				}
			}
		}
	}
	// Else
	else
	{
		// Iterate through all enemies in a visible radius	
		for (auto enemy : enemyUnits)
		{
			// If visible
			if (Broodwar->getUnit(enemy.first)->exists())
			{
				// Check if within radius
				if (enemy.second.getPosition().getDistance(unit->getPosition()) < radius)
				{
					if (enemy.second.getUnitType().isBuilding() && !enemy.second.getUnitType().canAttack())
					{
						thisUnit = 1.0 / (1.0 + double(unit->getDistance(enemy.second.getPosition())));
					}
					else if (enemy.second.getUnitType().isWorker())
					{
						// If it's an SCV that is repairing, make him priority
						if (unit->getType() == UnitTypes::Terran_SCV && unit->isRepairing())
						{
							thisUnit = 5.5;
						}
						else
						{
							thisUnit = 2.0 / (1.0 + double(unit->getDistance(enemy.second.getPosition())));
						}
					}
					else
					{
						thisUnit = 2.0 + (unitGetStrength(enemy.second.getUnitType()) / (1.0 + double(unit->getDistance(enemy.second.getPosition()))));
					}
					if (thisUnit > highest)
					{
						target = Broodwar->getUnit(enemy.first);
						highest = thisUnit;
					}
				}
			}
		}
	}
	// The highest strength unit is returned
	return target;
}

Unit getClusterTarget(Unit unit)
{
	// Cluster variables, range of spells is 10
	int highest = 0, range = 10;
	TilePosition clusterTile;

	// Reaver range is 8
	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		range = 8;
	}

	// Iterate through every tile in range to see what clusters are in range
	for (int x = unit->getTilePosition().x - range; x <= unit->getTargetPosition().x + range; x++)
	{
		for (int y = unit->getTilePosition().y - range; y <= unit->getTilePosition().y + range; y++)
		{
			if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight())
			{
				// For each unit, we only want clusters of specific targets
				// Reavers want ground clusters
				if (unit->getType() == UnitTypes::Protoss_Reaver)
				{
					if (clusterHeatmap[x][y] > highest)
					{
						highest = clusterHeatmap[x][y];
						clusterTile = TilePosition(x, y);
					}
				}
				// Arbiters want Siege Tank clusters
				else if (unit->getType() == UnitTypes::Protoss_Arbiter)
				{
					if (tankClusterHeatmap[x][y] > highest)
					{
						highest = clusterHeatmap[x][y];
						clusterTile = TilePosition(x, y);
					}
				}
				// High Templars can have air or ground clusters
				else if (unit->getType() == UnitTypes::Protoss_High_Templar)
				{
					if (clusterHeatmap[x][y] > highest)
					{
						highest = clusterHeatmap[x][y];
						clusterTile = TilePosition(x, y);
					}
				}
			}
		}
	}
	// If there is no cluster, return a getTarget unit
	if (highest < 2)
	{
		return getTarget(unit);
	}
	// Else if there is a cluster, ensure Reaver doesn't target a flying unit near the cluster tile
	else if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		return Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsFlyer);
	}
	// If there is no tank cluster, return NULL so Arbiter saves energy
	else if (unit->getType() == UnitTypes::Protoss_Arbiter)
	{
		return NULL;
	}
	return Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding);
}

#pragma endregion

#pragma region Movement

bool isThisACorner(Position position)
{
	// Given a position check -32 pixels to +32 pixels for being walkable mini tiles
	int x1 = (position.x - 32) / 8;
	int y1 = (position.y - 32) / 8;

	for (int i = x1; i <= x1 + 12; i++)
	{
		for (int j = y1; j <= y1 + 12; j++)
		{
			if (!Broodwar->isWalkable(i, j))
			{
				return true;
			}
		}
	}
	return false;
}

Position unitRegroup(Unit unit)
{
	// Currently not being used
	Unitset regroupSet = unit->getUnitsInRadius(50000, Filter::IsAlly && !Filter::IsBuilding && !Filter::IsWorker);
	Position regroupPosition;
	for (Unit re : regroupSet)
	{
		regroupPosition += re->getPosition();
	}
	if (regroupSet.size() > 0)
	{
		return (regroupPosition / regroupSet.size());
	}
	else
	{
		return Broodwar->getClosestUnit(unit->getPosition(), Filter::IsAlly && Filter::IsResourceDepot)->getPosition();
	}
}

Position unitFlee(Unit unit, Unit currentTarget)
{
	// If either the unit or current target are invalid, return
	if (!unit || !currentTarget)
	{
		return Positions::None;
	}

	// Unit Flee Variables
	double slopeDegree;
	int x, y;
	Position currentTargetPosition = currentTarget->getPosition();
	Position currentUnitPosition = unit->getPosition();
	TilePosition currentUnitTilePosition = unit->getTilePosition();

	// Divide by zero check, if zero then we are fleeing horizontally, no problem if fleeing vertically.
	if ((currentUnitPosition.x - currentTargetPosition.x) == 0)
	{
		slopeDegree = 1.571;
	}
	else
	{
		slopeDegree = atan((currentUnitPosition.y - currentTargetPosition.y) / (currentUnitPosition.x - currentTargetPosition.x));
	}

	// Need to make sure we are fleeing at the same angle the units are attacking each other at
	if (currentUnitPosition.x > currentTargetPosition.x)
	{
		x = (int)(currentUnitTilePosition.x + (5 * cos(slopeDegree)));
	}
	else
	{
		x = (int)(currentUnitTilePosition.x - (5 * cos(slopeDegree)));
	}
	if (currentUnitPosition.y > currentTargetPosition.y)
	{
		y = (int)(currentUnitTilePosition.y + abs(5 * sin(slopeDegree)));
	}
	else
	{
		y = (int)(currentUnitTilePosition.y - abs(5 * sin(slopeDegree)));
	}

	Position initialPosition = Position(TilePosition(x, y));
	// Spiral variables
	int length = 1;
	int j = 0;
	bool first = true;
	bool okay = false;
	int dx = 0;
	int dy = 1;
	// Searches in a spiral around the specified tile position
	while (length < 2000)
	{
		//If threat is low, move there
		if (x >= 0 && x < BWAPI::Broodwar->mapWidth() && y >= 0 && y < BWAPI::Broodwar->mapHeight())
		{
			Position newPosition = Position(TilePosition(x, y));
			if (enemyHeatmap[x][y] < 1 && (newPosition.getDistance(getNearestChokepoint(currentUnitPosition)->getCenter()) < 128 || (getRegion(currentUnitPosition)) == getRegion(newPosition) && !isThisACorner(newPosition)))
			{
				// Not a fully functional walkable check -- IMPLEMENTING
				if (Broodwar->isWalkable((x * 4), (y * 4)))
				{
					return newPosition;
				}
			}
		}
		//Otherwise, move to another position
		x = x + dx;
		y = y + dy;
		//Count how many steps we take in this direction
		j++;
		if (j == length) //if we've reached the end, its time to turn
		{
			//reset step counter
			j = 0;

			//Increment step counter
			if (!first)
				length++;

			//First=true for every other turn so we spiral out at the right rate
			first = !first;

			//Turn counter clockwise 90 degrees:
			if (dx == 0)
			{
				dx = dy;
				dy = 0;
			}
			else
			{
				dy = -dx;
				dx = 0;
			}
		}
	}
	return initialPosition;
}

Position unitGetPath(Unit unit, TilePosition target)
{
	int x = unit->getTilePosition().x;
	int y = unit->getTilePosition().y;
	int cnt = 0;
	//TilePosition target = Broodwar->getClosestUnit(enemyStartingPosition, Filter::IsMineralField)->getTilePosition();
	target = enemyStartingTilePosition;

	// Reset path
	for (int i = 0; i <= Broodwar->mapWidth(); i++)
	{
		for (int j = 0; j <= Broodwar->mapHeight(); j++)
		{
			if (shuttleHeatmap[i][j] < 256)
			{
				shuttleHeatmap[i][j] = 0;
			}
		}
	}

	shuttleHeatmap[x][y] = 0;
	shuttleHeatmap[target.x][target.y] = 1;

	// While current units tile is 0 (no path currently)
	while (shuttleHeatmap[x][y] < 1)
	{
		cnt++;
		// For each tile equal to cnt, move up and to the left, apply cnt + 1 to a 3x3 grid
		for (int i = 0; i <= Broodwar->mapWidth(); i++)
		{
			for (int j = 0; j <= Broodwar->mapHeight(); j++)
			{
				// If it's equal
				if (shuttleHeatmap[i][j] == cnt)
				{
					// Get 3x3 grid, set each tile to cnt + 1
					for (int a = i - 1; a <= i + 1; a++)
					{
						for (int b = j - 1; b <= j + 1; b++)
						{
							if (a >= 0 && a <= 256 && b >= 0 && b <= 256 && shuttleHeatmap[a][b] == 0)
							{
								shuttleHeatmap[a][b] = cnt + 1;
							}
						}
					}
				}
			}
		}
	}
	int lowestInt = 257;
	Position lowestPosition = unit->getPosition();
	for (int i = x - 3; i <= x + 4; i++)
	{
		for (int j = y - 3; j <= y + 4; j++)
		{
			if (airEnemyHeatmap[i][j] == 0 && shuttleHeatmap[i][j] > 0 && shuttleHeatmap[i][j] < lowestInt && i > 0 && i < Broodwar->mapWidth() && j > 0 && j < Broodwar->mapHeight())
			{
				lowestPosition = Position((i * 32) + 16, (j * 32) + 16);
				lowestInt = shuttleHeatmap[i][j];
			}
		}
	}
	return lowestPosition;
}

#pragma endregion

#pragma region SpecialUnits

void shuttleManager(Unit unit)
{
	bool harassing = false;
	if (find(shuttleID.begin(), shuttleID.end(), unit->getID()) == shuttleID.end())
	{
		shuttleID.push_back(unit->getID());
	}

	if (find(harassShuttleID.begin(), harassShuttleID.end(), unit->getID()) != harassShuttleID.end())
	{
		harassing = true;
	}

	if (unit->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsFlyer).size() >= 1)
	{
		Unitset loadedUnits = unit->getLoadedUnits();
		for (Unitset::iterator itr = loadedUnits.begin(); itr != loadedUnits.end(); itr++)
		{
			// If we are loaded and either under attack, cant unload or trying to retreat, move away
			if (unit->isUnderAttack() || !unit->canUnloadAll())
			{
				unit->move(unitFlee(unit, unit->getClosestUnit(Filter::IsEnemy)));
				return;
			}
			else if (Broodwar->getLatencyFrames() > (*itr)->getGroundWeaponCooldown() /*&& /(unitGetLocalStrategy(unit) == 1 || harassing == true)*/)
			{
				unit->unload(*itr);
				return;
			}
		}
		if (find(shuttleID.begin(), shuttleID.end(), unit->getID()) - shuttleID.begin() < (int)reaverID.size())
		{
			if (!unit->getTarget() && loadedUnits.size() < 1 && Broodwar->getLatencyFrames() + 30 >= Broodwar->getUnit(reaverID.at(find(shuttleID.begin(), shuttleID.end(), unit->getID()) - shuttleID.begin()))->getGroundWeaponCooldown())
			{
				Position correctedFleePosition = unitFlee(unit, unit->getClosestUnit(Filter::IsEnemy && !Filter::IsFlyer && !Filter::IsWorker && Filter::CanAttack));
				if (correctedFleePosition != BWAPI::Positions::None)
				{
					//Broodwar->drawLineMap(unit->getPosition(), correctedFleePosition, playerColor);
					unit->move(correctedFleePosition);
				}
			}
		}
	}
	else if (find(shuttleID.begin(), shuttleID.end(), unit->getID()) - shuttleID.begin() < (int)reaverID.size() && unit->getLoadedUnits().size() < 1)
	{
		unit->follow(Broodwar->getUnit(reaverID.at(find(shuttleID.begin(), shuttleID.end(), unit->getID()) - shuttleID.begin())));
	}
	else
	{
		unit->follow(Broodwar->getClosestUnit(enemyStartingPosition, Filter::GetType == UnitTypes::Protoss_Dragoon && Filter::IsAlly));
	}
}

void shuttleHarass(Unit unit)
{
	if (unit->getLoadedUnits().size() > 0 && (shuttleHeatmap[unit->getTilePosition().x][unit->getTilePosition().y] > 2 || shuttleHeatmap[unit->getTilePosition().x][unit->getTilePosition().y] == 0))
	{
		unit->move(unitGetPath(unit, enemyStartingTilePosition));
	}
	else
	{
		shuttleManager(unit);
	}
}

void reaverManager(Unit unit)
{
	if (unit->getScarabCount() < 10)
	{
		unit->train(UnitTypes::Protoss_Scarab);
	}
	unitGetCommand(unit);

	/*bool harassing = false;
	if (find(reaverID.begin(), reaverID.end(), unit->getID()) == reaverID.end())
	{
	reaverID.push_back(unit->getID());
	}
	if (find(harassReaverID.begin(), harassReaverID.end(), unit->getID()) != harassReaverID.end())
	{
	harassing = true;
	}

	if ((unitGetLocalStrategy(unit) == 0) || unit->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsFlyer).size() < 1 || Broodwar->getLatencyFrames() + 30 < unit->getGroundWeaponCooldown())
	{
	unitGetCommand(unit);
	// If a shuttle is following a unit, it is empty, see shuttle manager
	if (find(reaverID.begin(), reaverID.end(), unit->getID()) - reaverID.begin() < (int)shuttleID.size())
	{
	unit->rightClick(Broodwar->getUnit(shuttleID.at(find(reaverID.begin(), reaverID.end(), unit->getID()) - reaverID.begin())));
	}
	else if (unit->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsFlyer).size() > 0)
	{
	unitGetCommand(unit);
	}
	else
	{
	unitGetCommand(unit);
	}
	}
	else if (unitGetLocalStrategy(unit) == 1)
	{
	unitGetCommand(unit);
	}
	*/
}

void observerManager(Unit unit)
{
	if (invisibleUnits.size() > 0)
	{
		for (auto u : invisibleUnits)
		{
			unit->move(u.second);
			return;
		}
	}
	else
	{
		unit->move(supportPosition);
	}
}

void templarManager(Unit unit)
{
	Unit target = getClusterTarget(unit);
	int stratL = unitGetLocalStrategy(unit, target);
	if (stratL == 1 || stratL == 0)
	{
		if (target != unit)
		{
			if (unit->getEnergy() > 75)
			{
				unit->useTech(TechTypes::Psionic_Storm, target);
				return;
			}
			else if (unit->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_High_Templar) && (unit->getEnergy() < 70 || unit->isUnderAttack()))
			{
				unit->useTech(TechTypes::Archon_Warp, unit->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_High_Templar));
				return;
			}
		}
	}
	unit->move(supportPosition);
	return;
}

void arbiterManager(Unit unit)
{
	if (supportPosition != Positions::None && supportPosition != Positions::Unknown && supportPosition != Positions::Invalid)
	{
		unit->move(supportPosition);
	}
	else
	{
		unit->move(playerStartingPosition);
	}
	if (unit->getUnitsInRadius(640, Filter::IsEnemy).size() > 4)
	{
		Unit target = getClusterTarget(unit);
		if (target)
		{
			unit->useTech(TechTypes::Stasis_Field, target);
		}
	}
}

void carrierManager(Unit unit)
{
	if (unit->getInterceptorCount() < 8)
	{
		unit->train(UnitTypes::Protoss_Interceptor);
	}
	else
	{
		unitGetCommand(unit);
	}
}

void corsairManager(Unit unit)
{
	if (unit->getUnitsInRadius(320, Filter::IsEnemy && Filter::IsFlying).size() > 0)
	{
		unit->attack(unit->getClosestUnit(Filter::IsEnemy && Filter::IsFlying));
	}
	// If on tile of air threat > 0, move
	// Unit Micro, need air targeting
	// Need to move after every attack
	else
	{
		unit->move(unitGetPath(unit, enemyStartingTilePosition));
	}
}

#pragma endregion

#pragma region UnitTracking

int storeEnemyUnit(Unit unit, map<int, UnitInfo>& enemyUnits)
{
	int before = enemyUnits.size();
	UnitInfo newUnit(unit->getType(), unit->getPosition());
	enemyUnits.emplace(unit->getID(), newUnit);
	int after = enemyUnits.size();

	if (!unit->isVisible() && Broodwar->isVisible(TilePosition(enemyUnits.at(unit->getID()).getPosition())))
	{
		enemyUnits.at(unit->getID()).setPosition(Positions::None);
	}

	// If size is equal, check type
	if (before == after)
	{
		// If type changed, new unit discovered, add strength and return 1
		if (enemyUnits.at(unit->getID()).getUnitType() != unit->getType() && unit->isVisible())
		{
			enemyUnits.at(unit->getID()).setUnitType(unit->getType());
			return 1;
		}
		// If position changed, update for our local calculation usage
		if (enemyUnits.at(unit->getID()).getPosition() != unit->getPosition() && unit->isVisible())
		{
			enemyUnits.at(unit->getID()).setPosition(unit->getPosition());
		}
		return 0;
	}
	return 1;
	/*UnitInfo newUnit(unit->getType(), unit->getPosition());
	enemyUnits[unit->getID()] = newUnit;
	return 0;*/
}

int storeAllyUnit(Unit unit, map<int, UnitInfo>& allyUnits)
{
	UnitInfo newUnit(unit->getType(), unit->getPosition());
	allyUnits[unit->getID()] = newUnit;
	return 0;
}

UnitInfo::UnitInfo(UnitType newType, Position newPosition)
{
	unitType = newType;
	position = newPosition;
}

UnitInfo::UnitInfo()
{
	unitType = UnitTypes::Enum::None;
	position = Positions::None;
}

UnitInfo::~UnitInfo()
{
}

Position UnitInfo::getPosition() const
{
	return position;
}

UnitType UnitInfo::getUnitType() const
{
	return unitType;
}

void UnitInfo::setUnitType(UnitType newUnitType)
{
	unitType = newUnitType;
}

void UnitInfo::setPosition(Position newPosition)
{
	position = newPosition;
}

#pragma endregion