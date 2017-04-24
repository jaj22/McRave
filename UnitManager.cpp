#include "UnitManager.h"
#include "BWTA.h"

using namespace BWAPI;
using namespace std;
using namespace BWTA;

bool harassing = false;
bool stimResearched = false;

// Store units current command and what that command is doing (moving, where? enemy expansion/enemy main/enemy harass)
//		This storage can be used to count the strength of those units doing that command and see if it's sufficient to defend/attack that area
// Map invisible units to number of detectors moving to position?
// Add scout intercepting (based on velocity and direction?)
// Combat sim so there's no army swarming 1 unit

#pragma region Strategy

void unitMicro(Unit unit, Unit target)
{
	// Variables
	bool kite = false;
	int range = unitGetTrueRange(unit->getType(), Broodwar->self());
	int offset = 0;

	// Stop offset
	if (unit->getType() == UnitTypes::Protoss_Dragoon)
	{
		offset = 9;
	}
	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		offset = 1;
	}

	// Store when isAttackFrame was last true
	// That frame + 9 is locked down, do not allow commands during that time	

	if (unit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit->getTarget() == target)
	{
		return;
	}

	// If kiting unnecessary, disable
	if (target->getType().isBuilding() || unit->getType() == UnitTypes::Protoss_Corsair)
	{
		kite = false;
	}

	// If kiting is a good idea, enable
	else if (target->getType() == UnitTypes::Terran_Vulture_Spider_Mine || (range > 32 && unit->isUnderAttack()) || (target->getType().groundWeapon().maxRange() <= range && (unit->getDistance(target) < range - target->getType().groundWeapon().maxRange() && target->getType().groundWeapon().maxRange() > 0 && unit->getType().groundWeapon().maxRange() > 32 || unit->getHitPoints() < 40)))
	{
		kite = true;
	}

	// If kite is true and weapon on cooldown, move
	if (kite && unit->getGroundWeaponCooldown() > 0 && Broodwar->getFrameCount() - allyUnits[unit].getLastCommandFrame() > offset - Broodwar->getLatencyFrames())
	{
		Position correctedFleePosition = unitFlee(unit, target);
		// Want Corsairs to move closer always if possible
		if (unit->getType() == UnitTypes::Protoss_Corsair)
		{
			unit->move(target->getPosition());
			allyUnits[unit].setTargetPosition(target->getPosition());
		}
		else if (correctedFleePosition != BWAPI::Positions::None)
		{
			unit->move(Position(correctedFleePosition.x + rand() % 2 + (-1), correctedFleePosition.y + rand() % 2 + (-1)));
			allyUnits[unit].setTargetPosition(correctedFleePosition);
		}
	}
	// Else, regardless of if kite is true or not, attack if weapon is off cooldown
	else if (unit->getGroundWeaponCooldown() == 0)
	{
		unit->attack(target);		
	}
	return;
}

int unitGetGlobalStrategy()
{
	if (forceExpand == 1)
	{
		return 0;
	}

	if (allyStrength > enemyStrength)
	{
		// If Zerg, wait for a larger army before moving out
		if (Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) == 0)
		{
			return 0;
		}
		return 1;
	}
	else
	{		
		// If not Zerg, contain
		if (Broodwar->enemy()->getRace() != Races::Zerg)
		{
			return 1;
		}
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

	double thisUnit = 0.0;
	double enemyLocalStrength = 0.0, allyLocalStrength = 0.0;
	Position targetPosition = enemyUnits[target].getPosition();
	int radius = min(512, 384 + supply * 4);

	if (unit->getDistance(targetPosition) > 512)
	{
		return 3;
	}

	// Check every enemy unit being in range of the target
	for (auto u : enemyUnits)
	{
		// Reset unit strength
		thisUnit = 0.0;

		// Ignore workers, keep buildings (reinforcements and static defenses)
		if (u.second.getUnitType().isWorker() || (u.first && u.first->exists() && u.first->isStasised()))
		{
			continue;
		}

		// If a unit is within range of the target, add to local strength
		if (u.second.getPosition().getDistance(targetPosition) < radius)
		{			
			if (aLarge > 0 || aMedium > 0 || aSmall > 0)
			{
				// If unit is cloaked or burrowed and not detected, drastically increase strength
				if ((u.first->isCloaked() || u.first->isBurrowed()) && !u.first->isDetected())
				{
					thisUnit = 20 * u.second.getStrength();
				}
				else if (u.first->getType().groundWeapon().damageType() == DamageTypes::Explosive)
				{
					thisUnit = u.second.getStrength() * ((aLarge*1.0) + (aMedium*0.75) + (aSmall*0.5)) / (aLarge + aMedium + aSmall);
						//(((double(aLarge) * 1.0) + (double(aMedium) * 0.75) + (double(aSmall) * 0.5)) / double(aLarge + aMedium + aSmall));
				}
				else if (u.first->getType().groundWeapon().damageType() == DamageTypes::Concussive)
				{
					thisUnit = u.second.getStrength() * ((aLarge*1.0) + (aMedium*0.75) + (aSmall*0.5)) / (aLarge + aMedium + aSmall);
				}
				else
				{
					thisUnit = u.second.getStrength();
				}
			}
			else
			{
				thisUnit = u.second.getStrength();
			}
			// If enemy hasn't died, add to enemy. Otherwise, partially add to ally local
			if (u.second.getDeadFrame() == 0)
			{
				enemyLocalStrength += thisUnit;
			}
			else
			{
				allyLocalStrength += unitGetStrength(u.second.getUnitType()) * 1.0 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
			}
		}
	}

	// Check every ally being in range of the target
	for (auto u : allyUnits)
	{
		// Reset unit strength
		thisUnit = 0.0;

		// Ignore workers and buildings
		if (u.second.getUnitType().isWorker() || u.second.getUnitType().isBuilding())
		{
			continue;
		}

		// If a unit is within the range of the ally unit, add to local strength
		if (u.second.getPosition().getDistance(unit->getPosition()) < radius)
		{
			if (eLarge > 0 || eMedium > 0 || eSmall > 0)
			{
				// If shuttle, add units inside
				if (u.second.getUnitType() == UnitTypes::Protoss_Shuttle && u.first->getLoadedUnits().size() > 0)
				{
					// Assume reaver for damage type calculations
					for (Unit uL : u.first->getLoadedUnits())
					{
						thisUnit = allyUnits[uL].getStrength();
					}
				}
				else
				{
					// Damage type calculations
					if (u.second.getUnitType().groundWeapon().damageType() == DamageTypes::Explosive)
					{
						thisUnit = u.second.getStrength() * ((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / (eLarge + eMedium + eSmall);
					}
					else if (u.second.getUnitType().groundWeapon().damageType() == DamageTypes::Concussive)
					{
						thisUnit = u.second.getStrength() * ((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / (eLarge + eMedium + eSmall);
					}
					else
					{
						thisUnit = u.second.getStrength();
					}
				}
			}
			else
			{
				thisUnit = u.second.getStrength();
			}			

			// If ally hasn't died, add to ally. Otherwise, partially add to enemy local
			if (u.second.getDeadFrame() == 0)
			{
				allyLocalStrength += thisUnit;
			}
			else
			{
				enemyLocalStrength += unitGetStrength(u.second.getUnitType()) * 1.0 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
			}
		}
	}

	// Store the difference of strengths 
	allyUnits[unit].setLocal(allyLocalStrength - enemyLocalStrength);


	// If we are in ally territory and have a target, force to fight	
	if (target && target->exists())
	{
		if (allyTerritory.find(getRegion(target->getPosition())) != allyTerritory.end())
		{
			return 1;
		}
	}

	// Force Zealots to stay on Tanks
	if (unit->getType() == UnitTypes::Protoss_Zealot && target->exists() && (enemyUnits[target].getUnitType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || enemyUnits[target].getUnitType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && unit->getDistance(targetPosition) < 128)
	{
		return 1;
	}

	// If unit is in range of a target and not currently threatened, attack instead of running
	if (unit->getDistance(targetPosition) <= allyUnits[unit].getRange() && (enemyHeatmap[unit->getTilePosition().x][unit->getTilePosition().y] == 0 || unit->getType() == UnitTypes::Protoss_Reaver))
	{
		return 1;
	}

	// If last command was engage
	if (allyUnits[unit].getStrategy() == 1)
	{
		// Latch based system for at least 80% disadvantage to disengage
		if (allyLocalStrength < enemyLocalStrength*0.8)
		{
			allyUnits[unit].setStrategy(0);
			return 0;
		}
		allyUnits[unit].setStrategy(1);
		allyUnits[unit].setTargetPosition(targetPosition);
		return 1;
	}
	// If last command was disengage/no command
	else
	{
		// Latch based system for at least 120% advantage to engage
		if (allyLocalStrength >= enemyLocalStrength*1.2)
		{
			allyUnits[unit].setTargetPosition(targetPosition);
			allyUnits[unit].setStrategy(1);
			return 1;
		}
		// Otherwise return 3 or 0, whichever was previous
		allyUnits[unit].setStrategy(0);
		return 0;
	}
	// Disregard local if no target, no recent local calculation and not within ally region
	allyUnits[unit].setStrategy(3);
	return 3;
}

void unitGetCommand(Unit unit)
{
	allyUnits[unit].setTargetPosition(Positions::None);
	allyUnits[unit].setLocal(0);

	double closestD = 0.0;
	Position closestP;
	Unit target = unit;

	// Get target (based on priority and distance)
	// Get local decision (poking, front attack)
	// Get global decision (squads, harass, front attack)

	// Get target first
	if (unit->getType() == UnitTypes::Protoss_Reaver || unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		target = getClusterTarget(unit);
	}
	else
	{
		target = getTarget(unit);
	}



	// TESTING -- Getting target through class
	//target = allyUnits[unit].getTarget();	

	if (!target || target == nullptr)
	{
		return;
	}




	/* Check local strategy manager to see what our next task is.
	If 0, regroup unless forced to engage.
	If 1, send unit to micro-management. */

	int stratL = unitGetLocalStrategy(unit, target);

	// If target and unit are both valid and we're not ignoring local calculations
	if (stratL != 3)
	{
		// Attack
		if (stratL == 1 && target->exists() && target != unit)
		{
			unitMicro(unit, target);
			return;
		}
		// Retreat
		if (stratL == 0)
		{
			// If we are on top of our ramp, let's hold with zealots
			if (allyTerritory.size() <= 1 && unit->getDistance(defendHere.at(0)) < 64 && unit->getType() == UnitTypes::Protoss_Zealot)
			{
				if (unit->getUnitsInRadius(64, Filter::IsEnemy).size() > 0 && target != unit)
				{
					unitMicro(unit, target);
					return;
				}
				else if (forceExpand == 0 && unit->getDistance(defendHere.at(0)) > 64)
				{
					if (unit->getLastCommand().getTargetPosition().getDistance(defendHere.at(0)) > 5 || unit->getLastCommandFrame() + 10 < Broodwar->getFrameCount())
					{
						unit->move(Position(defendHere.at(0).x + rand() % 3 + (-1), defendHere.at(0).y + rand() % 3 + (-1)));
					}
					return;
				}
			}
			if (target && target->exists() && enemyHeatmap[unit->getTilePosition().x][unit->getTilePosition().y] > 0)
			{
				Position fleePosition = unitFlee(unit, target);
				if (fleePosition != Positions::None)
				{
					unit->move(Position(fleePosition.x + rand() % 3 + (-1), fleePosition.y + rand() % 3 + (-1)));
				}
				return;
			}

			for (auto position : defendHere)
			{
				if (unit->getDistance(position) < 320 && allyTerritory.find(getRegion(unit->getTilePosition())) != allyTerritory.end())
				{
					Position fleePosition = unitFlee(unit, target);
					if (fleePosition != Positions::None)
					{
						unit->move(Position(fleePosition.x + rand() % 3 + (-1), fleePosition.y + rand() % 3 + (-1)));
					}
					return;
				}
				if (unit->getDistance(position) <= closestD || closestD == 0.0)
				{
					closestD = unit->getDistance(position);
					closestP = position;
				}
			}
			if (unit->getLastCommand().getTargetPosition().getDistance(defendHere.at(0)) > 5 || unit->getLastCommandFrame() + 10 < Broodwar->getFrameCount())
			{
				unit->move(Position(defendHere.at(0).x + rand() % 3 + (-1), defendHere.at(0).y + rand() % 3 + (-1)));
			}
			return;
		}
	}


	/* Check our global strategy manager to see what our next task is.
	If 0, regroup at a chokepoint connected to allied territory.
	If 1, send units into a frontal attack.	*/
	int stratG = unitGetGlobalStrategy();

	if (stratG == 0)
	{
		if (enemyBasePositions.size() > 0 && allyTerritory.size() > 0)
		{
			// Pick random enemy bases to attack (cap at ~3-4 units?)
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
				if (allyTerritory.find(getNearestChokepoint(TilePosition(path.at(1)->Center()))->getRegions().second) != allyTerritory.end() || allyTerritory.find(getNearestChokepoint(TilePosition(path.at(1)->Center()))->getRegions().first) != allyTerritory.end())
				{
					closestP = Position(path.at(2)->Center());
				}
			}
			if (unit->getLastCommand().getTargetPosition().getDistance(closestP) > 5 || unit->getLastCommandFrame() + 100 < Broodwar->getFrameCount())
			{
				unit->move(Position(closestP.x + rand() % 3 + (-1), closestP.y + rand() % 3 + (-1)));
				return;
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
	if (stratG == 1 && enemyBasePositions.size() > 0)
	{
		if (target && target->exists() && unit->getDistance(target) < 512)
		{
			unitMicro(unit, target);
			return;
		}
		unit->attack(enemyBasePositions.front());
		return;
	}
}

#pragma endregion

#pragma region Strength

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
			return 20.0 * (1.0 + (range / 320.0)) * damage * (hp / 100.0) /** speed*/;
		}

		return 10.0 * (1.0 + (range / 320.0)) * damage * (hp / 100.0) /** speed*/;
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

	// If a unit is visible, we want to get its current strength based on current health, shield and any buffs
	if (unit->getType().isWorker() && getRegion(unit->getTilePosition()) == getRegion(playerStartingTilePosition))
	{
		if (unit->getPlayer() == Broodwar->self() && find(combatProbe.begin(), combatProbe.end(), unit) != combatProbe.end())
		{
			return 1.0;
		}
		if (unit->getPlayer() == Broodwar->enemy())
		{
			return 1.0;
		}
	}

	double hp = double(unit->getHitPoints() + (unit->getShields() / 2)) / double(unit->getType().maxHitPoints() + (unit->getType().maxShields() / 2));
	if (unit->isStimmed())
	{
		stimResearched = true;
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

#pragma endregion

#pragma region Targeting

Unit getTarget(Unit unit)
{
	double highest = 0.0, thisUnit = 0.0;

	Unit target = nullptr;

	for (auto u : enemyUnits)
	{
		if (!u.first)
		{
			continue;
		}

		// If unit is dead or unattackble based on flying
		if (u.second.getDeadFrame() > 0 || (u.second.getUnitType().isFlyer() && (unit->getType() == UnitTypes::Protoss_Zealot || unit->getType() == UnitTypes::Protoss_Reaver)))
		{
			continue;
		}

		if (u.first->exists() && u.first->isStasised())
		{
			continue;
		}
		
		double distance = 1.0 + double(unit->getDistance(u.second.getPosition()));

		if (u.first->exists())
		{
			thisUnit = unitGetStrength(u.second.getUnitType()) / distance;
		}
		else
		{
			thisUnit = 0.1*unitGetStrength(u.second.getUnitType()) / distance;
		}


		// Reduce building threat
		if (u.second.getUnitType().isBuilding())
		{
			thisUnit = 0.1*thisUnit;
		}		

		// If this is the strongest enemy around, target it
		if (thisUnit > highest || highest == 0)
		{
			target = u.first;
			highest = thisUnit;
		}
	}

	// If the target is not nullptr, store
	if (target)
	{
		allyUnits[unit].setTarget(target);
		allyUnits[unit].setTargetPosition(enemyUnits[target].getPosition());
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
				// Reavers want ground clusters
				if (unit->getType() == UnitTypes::Protoss_Reaver)
				{
					if (groundClusterHeatmap[x][y] > highest)
					{
						highest = groundClusterHeatmap[x][y];
						clusterTile = TilePosition(x, y);
					}
				}
				// Arbiters want Siege Tank clusters
				else if (unit->getType() == UnitTypes::Protoss_Arbiter)
				{
					if (tankClusterHeatmap[x][y] > highest)
					{
						highest = tankClusterHeatmap[x][y];
						clusterTile = TilePosition(x, y);
					}
				}
				// High Templars can have air or ground clusters
				else if (unit->getType() == UnitTypes::Protoss_High_Templar)
				{
					if (groundClusterHeatmap[x][y] > highest)
					{
						highest = groundClusterHeatmap[x][y];
						clusterTile = TilePosition(x, y);
					}
					if (airClusterHeatmap[x][y] > highest)
					{
						highest = airClusterHeatmap[x][y];
						clusterTile = TilePosition(x, y);
					}
				}
			}
		}
	}
	// If there is no cluster, return a getTarget unit
	if (highest < 2)
	{
		if (unit->getType() == UnitTypes::Protoss_Reaver)
		{
			return getTarget(unit);			
		}
		else
		{
			return nullptr;
		}
	}
	// Else if there is a cluster, ensure Reaver doesn't target a flying unit near the cluster tile
	else if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		return Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsFlyer, 128);
	}
	// If there is no tank cluster, return NULL so Arbiter saves energy
	else if (unit->getType() == UnitTypes::Protoss_Arbiter)
	{
		return Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding && (Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode), 128);
	}
	return Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding, 128);
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
			if (enemyHeatmap[x][y] < 1 && (newPosition.getDistance(getNearestChokepoint(currentUnitPosition)->getCenter()) < 128 || (getRegion(currentUnitPosition)) == getRegion(newPosition) && !isThisACorner(newPosition)) && Broodwar->getUnitsOnTile(TilePosition(x,y), Filter::IsAlly).size() < 2)
			{
				for (int i = 0; i <= 1; i++)
				{
					for (int j = 0; j <= 1; j++)
					{
						// Not a fully functional walkable check -- IMPLEMENTING
						if (Broodwar->isWalkable(((x * 4) + i), ((y * 4) + j)))
						{
							return newPosition + Position(i*8,j*8);
						}
					}
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
	target = TilePosition(playerStartingPosition);

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
		unit->move(unitGetPath(unit, TilePosition(playerStartingPosition)));
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
	// Find a position that is the highest concentration of units

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
		unit->move(unitGetPath(unit, TilePosition(playerStartingPosition)));
	}
}

#pragma endregion

#pragma region UnitTracking

int storeEnemyUnit(Unit unit, map<Unit, UnitInfo>& enemyUnits)
{
	// Create new unit
	if (enemyUnits.find(unit) == enemyUnits.end())
	{
		UnitInfo newUnit(unit->getType(), unit->getPosition(), unitGetVisibleStrength(unit), unitGetTrueRange(unit->getType(), Broodwar->enemy()), unit->getLastCommand().getType(), 0, 0, 0);
		enemyUnits[unit] = newUnit;
	}
	// Update unit
	else
	{
		enemyUnits[unit].setUnitType(unit->getType());
		enemyUnits[unit].setPosition(unit->getPosition());
		enemyUnits[unit].setStrength(unitGetVisibleStrength(unit));
		enemyUnits[unit].setRange(unitGetTrueRange(unit->getType(), Broodwar->enemy()));
	}
	return 0;
}
int storeAllyUnit(Unit unit, map<Unit, UnitInfo>& allyUnits)
{
	// Create new unit
	if (allyUnits.find(unit) == allyUnits.end())
	{
		UnitInfo newUnit(unit->getType(), unit->getPosition(), unitGetVisibleStrength(unit), unitGetTrueRange(unit->getType(), Broodwar->enemy()), unit->getLastCommand().getType(), 0, 0, 0);
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
	UnitInfo newUnit(unit->getType(), unit->getPosition(), unitGetVisibleStrength(unit), unitGetTrueRange(unit->getType(), Broodwar->self()), unit->getLastCommand().getType(), 0, 0, 0);
	allyUnits[unit] = newUnit;
	return 0;
}

UnitInfo::UnitInfo(UnitType newType, Position newPosition, double newStrength, double newRange, UnitCommandType newCommand, int newDeadFrame, int newStrategy, int newCommandFrame)
{
	unitType = newType;
	unitPosition = newPosition;
	unitStrength = newStrength;
	unitRange = newRange;
	unitCommand = newCommand;
	deadFrame = newDeadFrame;
	lastCommandFrame = newCommandFrame;
}
UnitInfo::UnitInfo()
{
	unitType = UnitTypes::Enum::None;
	unitPosition = Positions::None;
	unitStrength = 0.0;
	unitCommand = UnitCommandTypes::None;
}
UnitInfo::~UnitInfo()
{
}

Position UnitInfo::getPosition() const
{
	return unitPosition;
}
Position UnitInfo::getTargetPosition() const
{
	return targetPosition;
}
UnitType UnitInfo::getUnitType() const
{
	return unitType;
}
double UnitInfo::getStrength() const
{
	return unitStrength;
}
double UnitInfo::getRange() const
{
	return unitRange;
}
double UnitInfo::getLocal() const
{
	return unitLocal;
}
UnitCommandType UnitInfo::getCommand() const
{
	return unitCommand;
}
Unit UnitInfo::getTarget() const
{
	return target;
}
int UnitInfo::getDeadFrame() const
{
	return deadFrame;
}
int UnitInfo::getStrategy() const
{
	return strategy;
}
int UnitInfo::getLastCommandFrame() const
{
	return lastCommandFrame;
}

void UnitInfo::setUnitType(UnitType newUnitType)
{
	unitType = newUnitType;
}
void UnitInfo::setPosition(Position newPosition)
{
	unitPosition = newPosition;
}
void UnitInfo::setTargetPosition(Position newTargetPosition)
{
	targetPosition = newTargetPosition;
}
void UnitInfo::setStrength(double newStrength)
{
	unitStrength = newStrength;
}
void UnitInfo::setLocal(double newUnitLocal)
{
	unitLocal = newUnitLocal;
}
void UnitInfo::setRange(double newRange)
{
	unitRange = newRange;
}
void UnitInfo::setCommand(UnitCommandType newCommand)
{
	unitCommand = newCommand;
}
void UnitInfo::setTarget(Unit newTarget)
{
	target = newTarget;
}
void UnitInfo::setDeadFrame(int newDeadFrame)
{
	deadFrame = newDeadFrame;
}
void UnitInfo::setStrategy(int newStrategy)
{
	strategy = newStrategy;
}
void UnitInfo::setLastCommandFrame(int newCommandFrame)
{
	lastCommandFrame = newCommandFrame;
}
#pragma endregion


