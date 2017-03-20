#include "UnitManager.h"
#include "TargetManager.h"
#include "BWTA.h"

using namespace BWAPI;
using namespace std;
using namespace BWTA;

bool harassing = false;

int unitGetLocalStrategy(Unit unit)
{
	// Check for local strength, based on that make an adjustment
	//	Return 0 = retreat to holding position
	//  Return 1 = fight enemy
	//  Return 2 = disregard local calculations
	int radius = 32 * unit->getUnitsInRadius(320, Filter::IsAlly).size() + 500;
	double mod = 0.0;

	// Check if we are in ally territory, if so, fight	
	if (unit->getUnitsInRadius(radius, Filter::IsEnemy).size() > 0)
	{
		if (find(allyTerritory.begin(), allyTerritory.end(), BWTA::getRegion(unit->getClosestUnit(Filter::IsEnemy)->getPosition())) != allyTerritory.end())
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

	double enemyLocalStrength = 0.0, allyLocalStrength = 0.0;
	for (auto enemy : enemyUnits)
	{		
		Unit u = Broodwar->getUnit(enemy.first);
		if (enemy.second.getPosition().getDistance(unit->getPosition()) < radius)
		{
			// If unit is visible, get visible strength, else estimate strength
			if (u->isVisible())
			{
				// If unit is cloaked and not detected, make units very scared of them
				if ((u->isCloaked() || u->isBurrowed()) && !u->isDetected())
				{
					enemyLocalStrength += 20 * unitGetStrength(u->getType());
				}
				else if (u->getType().groundWeapon().damageType() == DamageTypes::Explosive)
				{
					enemyLocalStrength += unitGetVisibleStrength(u) * ((((double)aLarge * 1) + ((double)aMedium * 0.75) + ((double)aSmall * 0.5)) / (aLarge + aMedium + aSmall));
				}
				else if (u->getType().groundWeapon().damageType() == DamageTypes::Concussive)
				{
					enemyLocalStrength += unitGetVisibleStrength(u) * ((((double)aLarge * 0.25) + ((double)aMedium * 0.5) + ((double)aSmall * 1)) / (aLarge + aMedium + aSmall));
				}
				else
				{
					enemyLocalStrength += unitGetVisibleStrength(u);
				}
			}
			// Else used stored information
			else if (u->getType().groundWeapon().damageType() == DamageTypes::Explosive)
			{
				enemyLocalStrength += unitGetStrength(enemy.second.getUnitType()) * ((((double)aLarge * 1) + ((double)aMedium * 0.75) + ((double)aSmall * 0.5)) / (aLarge + aMedium + aSmall));
			}
			else if (u->getType().groundWeapon().damageType() == DamageTypes::Concussive)
			{
				enemyLocalStrength += unitGetStrength(enemy.second.getUnitType()) * ((((double)aLarge * 0.25) + ((double)aMedium * 0.5) + ((double)aSmall * 1)) / (aLarge + aMedium + aSmall));
			}
			else
			{
				enemyLocalStrength += unitGetStrength(enemy.second.getUnitType());
			}
		}

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
			// Damage type calculations
			if (ally->getType().groundWeapon().damageType() == DamageTypes::Explosive)
			{
				allyLocalStrength += unitGetVisibleStrength(ally) * ((((double)eLarge * 1) + ((double)eMedium * 0.75) + ((double)eSmall * 0.5)) / (eLarge + eMedium + eSmall));
			}
			else if (ally->getType().groundWeapon().damageType() == DamageTypes::Concussive)
			{
				allyLocalStrength += unitGetVisibleStrength(ally) * ((((double)eLarge * 0.25) + ((double)eMedium * 0.5) + ((double)eSmall * 1)) / (eLarge + eMedium + eSmall));
			}
			else
			{
				allyLocalStrength += unitGetVisibleStrength(ally);
			}
		}
	}
	// Include the unit itself into its calculations based on damage type
	if (unit->getType().groundWeapon().damageType() == DamageTypes::Explosive)
	{
		allyLocalStrength += unitGetVisibleStrength(unit) * ((((double)eLarge * 1) + ((double)eMedium * 0.75) + ((double)eSmall * 0.5)) / (eLarge + eMedium + eSmall));
	}
	else if (unit->getType().groundWeapon().damageType() == DamageTypes::Concussive)
	{
		allyLocalStrength += unitGetVisibleStrength(unit) * ((((double)eLarge * 0.25) + ((double)eMedium * 0.5) + ((double)eSmall * 1)) / (eLarge + eMedium + eSmall));
	}
	else
	{
		allyLocalStrength += unitGetVisibleStrength(unit);
	}


	// If there's enemies around, map information for HUD
	if (enemyLocalStrength > 0.0)
	{
		localEnemy[unit->getID()] = enemyLocalStrength;
		localAlly[unit->getID()] = allyLocalStrength;
		unitRadiusCheck[unit->getID()] = radius;
	}
	// Else remove information if it exists
	else if (localEnemy.find(unit->getID()) != localEnemy.end() && localAlly.find(unit->getID()) != localAlly.end() && unitRadiusCheck.find(unit->getID()) != unitRadiusCheck.end())
	{
		localEnemy.erase(unit->getID());
		localAlly.erase(unit->getID());
		unitRadiusCheck.erase(unit->getID());
	}

	// If ally higher, fight, else retreat
	if (enemyLocalStrength < allyLocalStrength)
	{
		unitsCurrentLocalCommand[unit] = 1;
		return 1;
	}
	else if (enemyLocalStrength >= allyLocalStrength)
	{
		unitsCurrentLocalCommand[unit] = 0;
		return 0;
	}
	// Else, disregard local
	return 2;
}

int unitGetGlobalStrategy()
{
	/*if (Broodwar->self()->supplyUsed() > 300)
	{
	return 1;
	}*/
	if (allyStrength > enemyStrength && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void unitGetCommand(Unit unit)
{
	/* Check local strategy manager to see what our next task is.
	If 0, regroup unless forced to engage.
	If 1, send unit to micro-management. */
	unitsCurrentTarget.erase(unit->getID());
	double closestD = 0;
	Position closestP;

	if (unitGetLocalStrategy(unit) == 0)
	{
		Position regroupPosition = unitRegroup(unit);
		// If we are on top of our ramp, let's hold with zealots
		if (unit->getDistance(defendHere.at(0)) < 64 && unit->getType() == UnitTypes::Protoss_Zealot && getRegion(unit->getPosition()) == getRegion(playerStartingPosition))
		{
			if (unit->getUnitsInRadius(64, Filter::IsEnemy).size() > 0)
			{
				unitMicro(unit);
				return;
			}
			else
			{
				unit->holdPosition();
				return;
			}
		}
		for (auto position : defendHere)
		{
			if (unit->getDistance(position) < 320)
			{
				unit->move(Broodwar->getClosestUnit(position, Filter::GetType == UnitTypes::Protoss_Nexus)->getPosition());
				return;
			}
			else if (unit->getDistance(position) <= closestD || closestD == 0.0)
			{
				closestD = unit->getDistance(position);
				closestP = position;
			}
		}
		unit->move(closestP);
	}
	// If fighting and there's an enemy around, micro
	else if (unitGetLocalStrategy(unit) == 1 && unit->getClosestUnit(Filter::IsEnemy && Filter::IsDetected))
	{		
		unitMicro(unit);
		return;
	}
	/* Check our global strategy manager to see what our next task is.
	If 0, regroup at a chokepoint connected to allied territory.
	If 1, send units into a frontal attack.	*/
	else if (unitGetGlobalStrategy() == 0)
	{
		if (enemyBasePositions.size() > 0 && allyTerritory.size() > 0)
		{
			closestD = 1000.0;
			closestP = defendHere.at(0);
			// Check if we are close enough to any defensive position
			for (auto position : defendHere)
			{
				if (unit->getDistance(position) < 320)
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
			if (unit->getLastCommand().getTargetPosition() != closestP)
			{
				unit->move(closestP);
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
	else if (unitGetGlobalStrategy() == 1)
	{
		if (unit->getClosestUnit(Filter::IsEnemy))
		{
			unitMicro(unit);
		}
		else if (enemyBasePositions.size() > 0)
		{
			unit->attack(enemyBasePositions.front());
			return;
		}
	}
}

void unitMicro(Unit unit)
{
	// Variables
	bool kite;
	int range = unit->getType().groundWeapon().maxRange();
	Unit currentTarget;
	
	// Get target priorities
	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		range = 256;
		currentTarget = getClusterTarget(unit);
	}
	else
	{
		currentTarget = getTarget(unit);
	}

	// Range upgrade check for Dragoons
	if (unit->getType() == UnitTypes::Protoss_Dragoon && Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
	{
		range = 192;
	}

	// If the target and unit exists, let's micro the unit
	if (currentTarget && unit && currentTarget != unit)
	{
		// Store current target position
		unitsCurrentTarget[unit->getID()] = currentTarget->getPosition();

		// If unit is attacking or in an attack frame, don't want to micro
		if (unit->isAttackFrame() || unit->isStartingAttack())
		{
			return;
		}

		// If recently issued a command to attack a unit, don't want to micro
		if (unit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit->getTarget() == currentTarget)
		{
			return;
		}

		// If kiting unnecessary, disable
		if (currentTarget->getType().isFlyer() || currentTarget->getType().isBuilding() || unit->getType() == UnitTypes::Protoss_Corsair)
		{
			kite = false;
		}

		// If kiting is a good idea, enable
		else if (currentTarget->getType() == UnitTypes::Terran_Vulture_Spider_Mine || (range > 32 && unit->isUnderAttack()) || (currentTarget->getType().groundWeapon().maxRange() <= range && (unit->getDistance(currentTarget) < range - currentTarget->getType().groundWeapon().maxRange() && currentTarget->getType().groundWeapon().maxRange() > 0 || unit->getHitPoints() < 40)))
		{
			kite = true;
		}

		// If kite is true and weapon on cooldown, move
		if (kite && Broodwar->getLatencyFrames() / 2 <= unit->getGroundWeaponCooldown())
		{
			Position correctedFleePosition = unitFlee(unit, currentTarget);
			// Want Corsairs to move closer always
			if (unit->getType() == UnitTypes::Protoss_Corsair)
			{
				unit->move(currentTarget->getPosition());
				unitsCurrentTarget[unit->getID()] = currentTarget->getPosition();
			}
			else if (correctedFleePosition != BWAPI::Positions::None)
			{
				unit->move(correctedFleePosition);
				unitsCurrentTarget[unit->getID()] = correctedFleePosition;
			}
		}
		// Else, regardless of if kite is true or not, attack if weapon is off cooldown
		else if (Broodwar->getLatencyFrames() / 2 > unit->getGroundWeaponCooldown())
		{
			unit->attack(currentTarget);
		}
	}
}

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
	if (unitType == UnitTypes::Protoss_Arbiter)
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
		double range, damage, hp;
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

		// Hp		
		hp = double(unitType.maxHitPoints() + (unitType.maxShields() / 2));

		return (1 + (range / 320.0)) * damage * (hp / 100);
	}
	return 0;
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
		return 2 * hp * unitGetStrength(unit->getType());
	}
	return hp * unitGetStrength(unit->getType());
}

double unitDamageMod(UnitType ally, UnitType enemy)
{
	if (ally.groundWeapon().damageType() == DamageTypes::Explosive)
	{
		if (enemy.size() == UnitSizeTypes::Small)
		{
			return 0.5;
		}
		else if (enemy.size() == UnitSizeTypes::Medium)
		{
			return 0.75;
		}
		else
		{
			return 1.0;
		}
	}
	else if (ally.groundWeapon().damageType() == DamageTypes::Concussive)
	{
		if (enemy.size() == UnitSizeTypes::Large)
		{
			return 0.25;
		}
		else if (enemy.size() == UnitSizeTypes::Medium)
		{
			return 0.5;
		}
		else
		{
			return 1.0;
		}
	}
	return 1.0;
}

Unit getTarget(Unit unit)
{
	// Check units only in local radius
	int radius = min(1280, 32 * (int)unit->getUnitsInRadius(320, Filter::IsAlly).size() + 400);
	double highest = 0.0, thisUnit = 0.0;
	Unit target;

	if (enemyAggresion)
	{
		return Broodwar->getClosestUnit(playerStartingPosition, Filter::IsEnemy);
	}

	// If Zealot, don't return air
	if (unit->getType() == UnitTypes::Protoss_Zealot || unit->getType() == UnitTypes::Protoss_Reaver)
	{
		target = unit->getClosestUnit(Filter::IsEnemy && !Filter::IsFlying);
		for (auto enemy : unit->getUnitsInRadius(radius, Filter::IsEnemy && !Filter::IsFlying))
		{
			if (enemy->getType().isWorker() || (enemy->getType().isBuilding() && enemy->canAttack()))
			{
				thisUnit = 0.1 / (1 + double(unit->getDistance(enemy))/32);
			}			
			else
			{
				thisUnit = unitGetStrength(enemy->getType()) / (1 + double(unit->getDistance(enemy))/32);
			}			
			if (thisUnit > highest)
			{
				target = enemy;
				highest = thisUnit;
			}			
		}
	}
	// If Corsair, don't return ground
	else if (unit->getType() == UnitTypes::Protoss_Corsair)
	{
		target = unit->getClosestUnit(Filter::IsEnemy && Filter::IsFlying);
		for (auto enemy : unit->getUnitsInRadius(radius, Filter::IsEnemy && Filter::IsFlying))
		{
			thisUnit = unitGetStrength(enemy->getType()) / (1 + double(unit->getDistance(enemy))/32);
			if (thisUnit > highest)
			{
				target = enemy;
				highest = thisUnit;
			}
		}
	}
	// Else return the strongest unit nearby
	else
	{
		target = unit->getClosestUnit(Filter::IsEnemy);
		for (auto enemy : unit->getUnitsInRadius(radius, Filter::IsEnemy))
		{
			if (enemy->getType().isWorker())
			{
				thisUnit = 1 / (1 + double(unit->getDistance(enemy)/32));
			}
			else
			{
				thisUnit = unitGetStrength(enemy->getType()) / (1 + double(unit->getDistance(enemy))/32);
			}
			if (thisUnit > highest)
			{
				target = enemy;
				highest = thisUnit;
			}
		}
	}	
	return target;
}

Unit getClusterTarget(Unit unit)
{
	int highest = 0, range = 10;
	TilePosition clusterTile;

	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		range = 8;
	}

	for (int x = unit->getTilePosition().x - range; x <= unit->getTargetPosition().x + range; x++)
	{
		for (int y = unit->getTilePosition().y + range; y <= unit->getTilePosition().y + range; y++)
		{
			if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight())
			{
				if (unit->getType() == UnitTypes::Protoss_Reaver)
				{
					if (clusterHeatmap[x][y] > highest && clusterHeatmap[x][y] > 0 && Broodwar->getUnitsOnTile(TilePosition(x, y), Filter::IsEnemy && !Filter::IsFlyer).size() > 0)
					{
						highest = clusterHeatmap[x][y];
						clusterTile = TilePosition(x, y);
					}
				}
				else if (unit->getType() == UnitTypes::Protoss_Arbiter)
				{
					if (clusterHeatmap[x][y] > highest && clusterHeatmap[x][y] > 0 && Broodwar->getUnitsOnTile(TilePosition(x, y), Filter::IsEnemy && (Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode)).size() > 0)
					{
						highest = clusterHeatmap[x][y];
						clusterTile = TilePosition(x, y);
					}
				}	
				else if (unit->getType() == UnitTypes::Protoss_High_Templar)
				{
					if (clusterHeatmap[x][y] > highest && clusterHeatmap[x][y] > 0 && Broodwar->getUnitsOnTile(TilePosition(x, y), Filter::IsEnemy).size() > 0)
					{
						highest = clusterHeatmap[x][y];
						clusterTile = TilePosition(x, y);
					}
				}
			}
		}
	}
	if (highest < 2)
	{
		return getTarget(unit);
	}
	else if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		return Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsFlyer);
	}
	return Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding);
}

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
	if (!unit || !currentTarget)
	{
		return BWAPI::Positions::None;
	}

	double slopeDegree;
	int x, y;
	Position currentTargetPosition = currentTarget->getPosition();

	// Divide by zero check, if zero then we are fleeing horizontally, no problem if fleeing vertically.
	if ((unit->getPosition().x - currentTargetPosition.x) == 0)
	{
		slopeDegree = 1.571;
	}

	else
	{
		slopeDegree = atan((unit->getPosition().y - currentTargetPosition.y) / (unit->getPosition().x - currentTargetPosition.x));
	}

	// Need to make sure we are fleeing in the correct direction
	if (unit->getPosition().x > currentTargetPosition.x)
	{
		x = (int)(5 * cos(slopeDegree)) + unit->getTilePosition().x;
	}
	else
	{
		x = (int)(unit->getTilePosition().x - (5 * cos(slopeDegree)));
	}
	if (unit->getPosition().y > currentTargetPosition.y)
	{
		y = (int)(unit->getTilePosition().y + abs(5 * sin(slopeDegree)));
	}
	else
	{
		y = (int)(unit->getTilePosition().y - abs(5 * sin(slopeDegree)));
	}

	Position initialPosition = Position(x * 32, y * 32);
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
			if (enemyHeatmap[x][y] < 1 && (Position(x * 32, y * 32).getDistance(getNearestChokepoint(unit->getPosition())->getCenter()) < 128 || (BWTA::getRegion(unit->getPosition()) == getRegion(Position(x * 32, y * 32)) && !isThisACorner(Position(x * 32, y * 32)))))
			{
				if (Broodwar->isWalkable((x * 4), (y * 4)))
				{
					return BWAPI::Position(x * 32, y * 32);
				}

				//// Check if one of the tiles is walkable
				//for (int i = 0; i <= 1; i++)
				//{
				//	for (int j = 0; j <= 1; j++)
				//	{
				//		if (Broodwar->isWalkable((x * 4) + i, (y * 4) + j))
				//		{
				//			return BWAPI::Position(x * 32, y * 32);
				//		}
				//	}
				//}				
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
			else if (Broodwar->getLatencyFrames() > (*itr)->getGroundWeaponCooldown() && (unitGetLocalStrategy(unit) == 1 || harassing == true))
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
					Broodwar->drawLineMap(unit->getPosition(), correctedFleePosition, playerColor);
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
	if (unit->getUnitsInRadius(1280, Filter::IsEnemy && (Filter::IsBurrowed || Filter::IsCloaked)).size() > 0)
	{
		unit->move(unit->getClosestUnit(Filter::IsEnemy && (Filter::IsBurrowed || Filter::IsCloaked))->getPosition());
	}
	else
	{
		unit->move(supportPosition);
	}
}

void templarManager(Unit unit)
{
	if (unit->getUnitsInRadius(320, Filter::IsEnemy).size() < 4)
	{
		unit->move(supportPosition);
	}
	else
	{
		if (unit->getEnergy() < 70 || unit->isUnderAttack())
		{
			unit->useTech(TechTypes::Archon_Warp, unit->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_High_Templar));
		}
		Unit target = getClusterTarget(unit);
		if (target)
		{
			unit->useTech(TechTypes::Psionic_Storm, target);
		}
	}
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

	// If size changed, new unit discovered, add strength and return 1
	/*enemyStrength += unitGetStrength(unit->getType);*/
	return 1;
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
