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
	int radius = 32*unit->getUnitsInRadius(320, Filter::IsAlly).size() + 400;

	// Check if we are in ally territory, if so, fight	
	if (unit->getUnitsInRadius(radius, Filter::IsEnemy && !Filter::IsWorker).size() > 0)
	{		
		if (find(allyTerritory.begin(), allyTerritory.end(), BWTA::getRegion(unit->getClosestUnit(Filter::IsEnemy)->getPosition())) != allyTerritory.end())
		{
			return 1;
		}
	}

	// If we don't have a Cyber core yet, we don't want to push out of the base
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) < 1)
	{
		return 2;
	}

	double enemyLocalStrength = 0.0, allyLocalStrength = 0.0;
	for (auto enemy : enemyUnits)
	{
		if (enemy.second.getPosition().getDistance(unit->getPosition()) < radius)
		{
			// If unit is visible, get visible strength, else estimate strength
			if (Broodwar->getUnit(enemy.first)->isVisible())
			{
				enemyLocalStrength += unitGetVisibleStrength(Broodwar->getUnit(enemy.first));
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
			for (Unit u : ally->getLoadedUnits())
			{
				allyLocalStrength += unitGetVisibleStrength(u);
			}
		}
		else
		{
			allyLocalStrength += unitGetVisibleStrength(ally);
		}
	}
	// Add the unit we are looking at (prevents indecision of some units)	
	allyLocalStrength += unitGetVisibleStrength(unit);
	

	// If there's enemies around, map information for HUD
	if (enemyLocalStrength > 0.0)
	{
		localEnemy[unit->getID()] = enemyLocalStrength;
		localAlly[unit->getID()] = allyLocalStrength;
		unitRadiusCheck.emplace(unit->getID(), radius);
	}
	// Else remove information if it exists
	else if (localEnemy.find(unit->getID()) != localEnemy.end() && localAlly.find(unit->getID()) != localAlly.end() && unitRadiusCheck.find(unit->getID()) != unitRadiusCheck.end())
	{
		localEnemy.erase(unit->getID());
		localAlly.erase(unit->getID());
		unitRadiusCheck.erase(unit->getID());
	}

	// If ally higher, fight, else retreat
	if (enemyLocalStrength < allyLocalStrength && enemyLocalStrength > 0)
	{		
		return 1;
	}
	else if (enemyLocalStrength >= allyLocalStrength && enemyLocalStrength > 0)
	{
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
	if (unitGetLocalStrategy(unit) == 0)
	{
		Position regroupPosition = unitRegroup(unit);
		// If we are on top of our ramp, let's hold with zealots
		if (unit->getDistance(defendHere.at(0)) < 32 && unit->getType() == UnitTypes::Protoss_Zealot && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot) > 2 && BWTA::getRegion(unit->getPosition()) == BWTA::getRegion(playerStartingPosition) && Broodwar->self()->supplyUsed() < 80)
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
		}
		unit->move(Position(defendHere.at(rand() % defendHere.size()).x + rand() % 100 - 50, defendHere.at(rand() % defendHere.size()).y + rand() % 100 - 50));
		return;
	}
	// If fighting and there's an enemy around, micro
	else if (unitGetLocalStrategy(unit) == 1 && unit->getClosestUnit(Filter::IsEnemy))
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
			// Check if we are close enough to any defensive position
			for (auto position : defendHere)
			{
				if (unit->getDistance(position) < 200)
				{
					return;
				}
			}
			// Else defend at one of the vulnerable chokepoints
			unit->move(Position(defendHere.at(rand() % defendHere.size()).x + rand() % 100 - 50, defendHere.at(rand() % defendHere.size()).y + rand() % 100 - 50));
			return;
		}
		else
		{
			// Else just defend at nearest chokepoint (starting of game without scout information)
			unit->move((BWTA::getNearestChokepoint(unit->getPosition()))->getCenter());
			return;
		}
	}

	// Check if we should attack
	else if (unitGetGlobalStrategy() == 1)
	{
		if (unit->getUnitsInRadius(640, Filter::IsEnemy).size() > 0)
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

double unitGetStrength(UnitType unitType)
{
	// Some hardcoded values
	if (unitType == UnitTypes::Terran_Bunker || unitType == UnitTypes::Zerg_Sunken_Colony)
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

		// Zealot has to be doubled
		if (unitType == UnitTypes::Protoss_Zealot)
		{
			damage = damage * 2;
		}

		// Hp		
		hp = double(unitType.maxHitPoints() + (unitType.maxShields() / 2));

		return sqrt(1 + (range / 320.0)) * damage * (hp / 100);
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
	double hp = double(unit->getHitPoints() + (unit->getShields() / 2)) / double(unit->getType().maxHitPoints() + (unit->getType().maxShields() / 2));
	return hp * unitGetStrength(unit->getType());
}

Position unitGetPath(Unit unit)
{
	int x = unit->getTilePosition().x;
	int y = unit->getTilePosition().y;
	int cnt = 0;
	//TilePosition target = Broodwar->getClosestUnit(enemyStartingPosition, Filter::IsMineralField)->getTilePosition();
	TilePosition target = enemyStartingTilePosition;

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

void unitMicro(Unit unit)
{
	// Variables
	bool kite;
	int range = unit->getType().groundWeapon().maxRange();
	Unit currentTarget;

	// Get target priorities
	if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		currentTarget = clusterTargetPriority(unit);
	}
	else if (unit->getType() == UnitTypes::Protoss_Zealot)
	{
		currentTarget = groundTargetPriority(unit);
	}
	else
	{
		currentTarget = targetPriority(unit);
	}

	// Range upgrade check for Dragoons
	if (unit->getType() == UnitTypes::Protoss_Dragoon && Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
	{
		range = 192;
	}

	// If the target and unit exists, let's micro the unit
	if (currentTarget && unit)
	{
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

		// If units too far away, move back
		if (unit->getDistance(unit->getClosestUnit(Filter::IsAlly && Filter::IsBuilding)) > 640 && unitGetGlobalStrategy == 0)
		{
			unit->move((Position(defendHere.at(rand() % defendHere.size()).x + rand() % 100, defendHere.at(rand() % defendHere.size()).y + rand() % 100)));
		}

		// If kiting unnecessary, disable
		if (currentTarget->getType().isFlyer() || currentTarget->getType().isBuilding() /*|| !currentTarget->canAttack()*/)
		{
			kite = false;
		}

		// If kiting is a good idea, enable
		else if (currentTarget->getType() == UnitTypes::Terran_Vulture_Spider_Mine || (range > 32 && unit->isUnderAttack()) || (currentTarget->getType().groundWeapon().maxRange() <= range && (unit->getDistance(currentTarget) < range - currentTarget->getType().groundWeapon().maxRange() || unit->getHitPoints() < 40)))
		{
			kite = true;
		}

		// If kite is true and weapon on cooldown, move
		if (kite && unit->getLastCommand().getType() != UnitCommandTypes::Move && Broodwar->getLatencyFrames() / 2 <= unit->getGroundWeaponCooldown())
		{
			Position correctedFleePosition = unitFlee(unit, currentTarget);
			if (correctedFleePosition != BWAPI::Positions::None)
			{
				Broodwar->drawLineMap(unit->getPosition(), correctedFleePosition, playerColor);
				unit->move(correctedFleePosition);
			}
		}
		// Else, regardless of if kite is true or not, attack if weapon is off cooldown
		else if (Broodwar->getLatencyFrames() / 2 > unit->getGroundWeaponCooldown())
		{
			unit->attack(currentTarget);
		}
	}
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
	int dx = 0;
	int dy = 1;
	// Searches in a spiral around the specified tile position
	while (length < 2000)
	{
		//If threat is low, move there
		if (x >= 0 && x < BWAPI::Broodwar->mapWidth() && y >= 0 && y < BWAPI::Broodwar->mapHeight())
		{
			if (Broodwar->getUnitsInRadius(Position(x * 32, y * 32), 128, Filter::IsEnemy).size() < 1 && enemyHeatmap[x][y] < 1 && (Position(x * 32, y * 32).getDistance(getNearestChokepoint(unit->getPosition())->getCenter()) < 128 || (BWTA::getRegion(unit->getPosition()) == getRegion(Position(x * 32, y * 32)) && !isThisACorner(Position(x * 32, y * 32)))))
			{
				return BWAPI::Position(x * 32, y * 32);
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
			else if (Broodwar->getLatencyFrames() > (*itr)->getGroundWeaponCooldown() && (unitGetLocalStrategy(unit) == 1 || forceEngage == true || harassing == true))
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
		unit->move(unitGetPath(unit));
	}
	else
	{
		shuttleManager(unit);
	}
}

void reaverManager(Unit unit)
{
	/*bool harassing = false;*/
	if (find(reaverID.begin(), reaverID.end(), unit->getID()) == reaverID.end())
	{
		reaverID.push_back(unit->getID());
	}
	/*if (find(harassReaverID.begin(), harassReaverID.end(), unit->getID()) != harassReaverID.end())
	{
	harassing = true;
	}*/
	if (unit->getScarabCount() < 10)
	{
		unit->train(UnitTypes::Protoss_Scarab);
	}
	if ((unitGetLocalStrategy(unit) == 0 && forceEngage == 0) || unit->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsFlyer).size() < 1 || Broodwar->getLatencyFrames() + 30 < unit->getGroundWeaponCooldown())
	{
		// If a shuttle is following a unit, it is empty, see shuttle manager
		if (find(reaverID.begin(), reaverID.end(), unit->getID()) - reaverID.begin() < (int)shuttleID.size())
		{
			unit->rightClick(Broodwar->getUnit(shuttleID.at(find(reaverID.begin(), reaverID.end(), unit->getID()) - reaverID.begin())));
		}
		else if (unit->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsFlyer).size() > 0)
		{
			unitMicro(unit);
		}
		else
		{
			unitGetCommand(unit);
		}
	}
	else if (unitGetLocalStrategy(unit) == 1 || forceEngage == 1)
	{
		unitGetCommand(unit);
	}
}

void observerManager(Unit unit)
{
	Unit currentTarget;
	if (unit->isIdle())
	{
		if (unit->getUnitsInRadius(640, Filter::IsEnemy && (Filter::IsBurrowed || Filter::IsCloaked)).size() > 0)
		{
			currentTarget = unit->getClosestUnit(Filter::IsEnemy && (Filter::IsBurrowed || Filter::IsCloaked));
		}
		else
		{
			currentTarget = Broodwar->getClosestUnit(enemyStartingPosition, Filter::IsAlly && Filter::GetType == UnitTypes::Enum::Protoss_Dragoon);
		}
		if (currentTarget)
		{
			unit->follow(currentTarget);
		}
		else
		{
			unit->follow(unit->getClosestUnit(Filter::IsAlly));
		}
	}
}

void templarManager(Unit unit)
{
	if (unit->getUnitsInRadius(320, Filter::IsEnemy).size() < 4)
	{
		unit->follow(Broodwar->getClosestUnit(enemyStartingPosition, Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Dragoon));
	}
	else
	{
		Unit target = clusterTargetPriority(unit);
		if (target)
		{
			unit->useTech(TechTypes::Psionic_Storm, target);
		}
	}
}

void arbiterManager(Unit unit)
{
	if (arbiterPosition != Positions::None && arbiterPosition != Positions::Unknown && arbiterPosition != Positions::Invalid)
	{
		unit->move(arbiterPosition);
	}
	else
	{
		unit->move(playerStartingPosition);
	}
	if (unit->getUnitsInRadius(640, Filter::IsEnemy).size() > 4)
	{
		Unit target = clusterTargetPriority(unit);
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