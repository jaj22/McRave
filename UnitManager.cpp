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

void UnitTrackerClass::unitMicroTarget(Unit unit, Unit target)
{
	// Variables
	bool kite = false;
	int range = allyUnits[unit].getRange();
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
	if (kite && Broodwar->getFrameCount() - allyUnits[unit].getLastCommandFrame() > offset - Broodwar->getRemainingLatencyFrames() && unit->getGroundWeaponCooldown() > 0)
	{
		Position correctedFleePosition = unitFlee(unit, target);
		// Want Corsairs to move closer always if possible
		if (unit->getType() == UnitTypes::Protoss_Corsair)
		{
			unit->move(target->getPosition());
			allyUnits[unit].setTargetPosition(target->getPosition());
		}
		else if (correctedFleePosition != BWAPI::Positions::None && (unit->getLastCommand().getType() != UnitCommandTypes::Move || unit->getLastCommand().getTargetPosition().getDistance(correctedFleePosition) > 5))
		{
			unit->move(Position(correctedFleePosition.x + rand() % 3 + (-1), correctedFleePosition.y + rand() % 3 + (-1)));
			allyUnits[unit].setTargetPosition(correctedFleePosition);
		}
	}
	// Else, regardless of if kite is true or not, attack if weapon is off cooldown
	else if (unit->getGroundWeaponCooldown() <= 0)
	{
		unit->attack(target);
	}
	return;
}

void UnitTrackerClass::unitExploreArea(Unit unit)
{
	// Given a region, explore a random portion of it based on random metrics like:
	// Distance to enemy
	// Distance to home
	// Last explored
	// Unit deaths
	// Untouched resources
}

int UnitTrackerClass::unitGetGlobalStrategy()
{
	if (forceExpand == 1)
	{
		return 0;
	}

	if (allyStrength > enemyStrength)
	{
		// If Zerg, wait for a larger army before moving out
		if (Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) == 0)
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
	return 1;
}

void UnitTrackerClass::unitGetLocalStrategy(Unit unit, Unit target)
{
	double thisUnit = 0.0;
	double enemyLocalStrength = 0.0, allyLocalStrength = 0.0;
	Position targetPosition = enemyUnits[target].getPosition();
	int radius = min(512, 384 + supply * 4);

	if (unit->getDistance(targetPosition) > 512)
	{
		allyUnits[unit].setStrategy(3);
		return;
	}

	// Check every enemy unit being in range of the target
	for (auto u : UnitTracker::Instance().getEnUnits())
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
				allyLocalStrength += u.second.getMaxStrength() * 1.0 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
			}
		}
	}

	// Check every ally being in range of the target
	for (auto u : UnitTracker::Instance().getMyUnits())
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
				enemyLocalStrength += u.second.getMaxStrength() * 1.0 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
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
			allyUnits[unit].setStrategy(1);
			return;
		}
	}

	// Force Zealots to stay on Tanks
	if (unit->getType() == UnitTypes::Protoss_Zealot && target->exists() && (enemyUnits[target].getUnitType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || enemyUnits[target].getUnitType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && unit->getDistance(targetPosition) < 128)
	{
		allyUnits[unit].setStrategy(1);
		return;
	}

	// If unit is in range of a target and not currently threatened, attack instead of running
	if (unit->getDistance(targetPosition) <= allyUnits[unit].getRange() && (enemyGroundStrengthGrid[unit->getTilePosition().x][unit->getTilePosition().y] == 0 || unit->getType() == UnitTypes::Protoss_Reaver))
	{
		allyUnits[unit].setStrategy(1);
		return;
	}

	// If last command was engage
	if (allyUnits[unit].getStrategy() == 1)
	{
		// Latch based system for at least 80% disadvantage to disengage
		if (allyLocalStrength < enemyLocalStrength*0.8)
		{
			allyUnits[unit].setStrategy(0);
			return;
		}
		allyUnits[unit].setStrategy(1);
		allyUnits[unit].setTargetPosition(targetPosition);
		return;
	}
	// If last command was disengage/no command
	else
	{
		// Latch based system for at least 120% advantage to engage
		if (allyLocalStrength >= enemyLocalStrength*1.2)
		{
			allyUnits[unit].setTargetPosition(targetPosition);
			allyUnits[unit].setStrategy(1);
			return;
		}
		// Otherwise return 3 or 0, whichever was previous
		allyUnits[unit].setStrategy(0);
		return;
	}
	// Disregard local if no target, no recent local calculation and not within ally region
	allyUnits[unit].setStrategy(3);
	return;
}

void UnitTrackerClass::unitGetCommand(Unit unit)
{
	allyUnits[unit].setTargetPosition(Positions::None);
	allyUnits[unit].setLocal(0);
	unitGetTarget(unit);

	double closestD = 0.0;
	Position closestP;
	Unit target = allyUnits[unit].getTarget();

	if (!target || target == nullptr)
	{
		return;
	}

	if (unit->getType() == UnitTypes::Protoss_Reaver && unit->getScarabCount() < 5)
	{
		unit->train(UnitTypes::Protoss_Scarab);
	}

	unitGetLocalStrategy(unit, target);
	int stratG = unitGetGlobalStrategy();
	int stratL = allyUnits[unit].getStrategy();

	// If target and unit are both valid and we're not ignoring local calculations
	if (stratL != 3)
	{
		// Attack
		if (stratL == 1 && target->exists())
		{
			unitMicroTarget(unit, target);
			return;
		}
		// Retreat
		if (stratL == 0)
		{
			// Force engage Zealots on ramp
			if (allyTerritory.size() <= 1 && unit->getDistance(defendHere.at(0)) < 64 && unit->getType() == UnitTypes::Protoss_Zealot && unit->getUnitsInRadius(64, Filter::IsEnemy).size() > 0)
			{
				unitMicroTarget(unit, target);
				return;
			}

			// Create concave when containing units
			if (enemyGroundStrengthGrid[unit->getTilePosition().x][unit->getTilePosition().y] == 0.0 && stratG == 1)
			{
				Position fleePosition = unitFlee(unit, target);
				if (fleePosition != Positions::None)
				{
					unit->move(Position(fleePosition.x + rand() % 3 + (-1), fleePosition.y + rand() % 3 + (-1)));
				}
				return;
			}

			// For each defensive position, find closest one
			for (auto position : defendHere)
			{
				if (unit->getDistance(position) < 320 || allyTerritory.find(getRegion(unit->getTilePosition())) != allyTerritory.end())
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

			// If last command was too far away from this position, move there
			if (unit->getLastCommand().getTargetPosition().getDistance(defendHere.at(0)) > 5)
			{
				unit->move(Position(defendHere.at(0).x + rand() % 3 + (-1), defendHere.at(0).y + rand() % 3 + (-1)));
			}
			return;
		}
	}

	if (stratG == 0)
	{
		if (enemyBasePositions.size() > 0 && allyTerritory.size() > 0)
		{
			// Pick random enemy bases to attack (cap at ~3-4 units?)
			closestD = 1000.0;
			closestP = defendHere.at(0);

			// If we forced to expand, move to next choke to prevent blocking 
			if (forceExpand == 1)
			{
				closestP = Position(path.at(1)->Center());
				if (allyTerritory.find(getNearestChokepoint(TilePosition(path.at(1)->Center()))->getRegions().second) != allyTerritory.end() || allyTerritory.find(getNearestChokepoint(TilePosition(path.at(1)->Center()))->getRegions().first) != allyTerritory.end())
				{
					closestP = Position(path.at(2)->Center());
				}
			}
			else
			{
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
			UnitTrackerClass::unitMicroTarget(unit, target);
			return;
		}
		unit->attack(enemyBasePositions.front());
		return;
	}
}

void UnitTrackerClass::unitGetTarget(Unit unit)
{
	double highest = 0.0, thisUnit = 0.0;

	Unit target = nullptr;

	for (auto &u : enemyUnits)
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
			thisUnit = u.second.getMaxStrength() / distance;
		}
		else
		{
			thisUnit = 0.1*u.second.getMaxStrength() / distance;
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
	return;
}

void UnitTrackerClass::unitGetClusterTarget(Unit unit)
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
					if (enemyGroundClusterGrid[x][y] > highest)
					{
						highest = enemyGroundClusterGrid[x][y];
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
					if (enemyGroundClusterGrid[x][y] > highest)
					{
						highest = enemyGroundClusterGrid[x][y];
						clusterTile = TilePosition(x, y);
					}
					if (enemyAirClusterGrid[x][y] > highest)
					{
						highest = enemyAirClusterGrid[x][y];
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
			return UnitTrackerClass::unitGetTarget(unit);
		}
		else
		{
			return;
		}
	}
	// Return ground cluster for Reavers
	else if (unit->getType() == UnitTypes::Protoss_Reaver)
	{
		allyUnits[unit].setTarget(Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsFlyer, 128));
	}
	// Return tank cluster for Arbiters
	else if (unit->getType() == UnitTypes::Protoss_Arbiter)
	{
		allyUnits[unit].setTarget(Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding && (Filter::GetType == UnitTypes::Terran_Siege_Tank_Tank_Mode || Filter::GetType == UnitTypes::Terran_Siege_Tank_Siege_Mode), 128));
	}
	// Return unit cluster for High Templars
	else if (unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		allyUnits[unit].setTarget(Broodwar->getClosestUnit(Position(clusterTile), Filter::IsEnemy && !Filter::IsBuilding, 128));
	}
	return;
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

void UnitTrackerClass::unitUpdate(Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{
		storeAllyUnit(unit, allyUnits);	
	}
	else
	{
		storeEnemyUnit(unit, enemyUnits);
	}

	// If unit has been dead for over 500 frames, erase it (needed manual loop)
	for (map<Unit, UnitInfoClass>::iterator itr = allyUnits.begin(); itr != allyUnits.end();)
	{
		if (itr->second.getDeadFrame() != 0 && itr->second.getDeadFrame() + 500 < Broodwar->getFrameCount())
		{
			itr = allyUnits.erase(itr);
		}
		else
		{
			++itr;
		}
	}
	// If unit has been dead for over 500 frames, erase it (needed manual loop)
	for (map<Unit, UnitInfoClass>::iterator itr = enemyUnits.begin(); itr != enemyUnits.end();)
	{
		if ((*itr).second.getDeadFrame() != 0 && (*itr).second.getDeadFrame() + 500 < Broodwar->getFrameCount() || itr->first && itr->first->exists() && itr->first->getPlayer() != Broodwar->enemy())
		{
			itr = enemyUnits.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

void UnitTrackerClass::unitDeath(Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{
		allyUnits[unit].setDeadFrame(Broodwar->getFrameCount());		
	}
	else
	{
		enemyUnits[unit].setDeadFrame(Broodwar->getFrameCount());
	}
}

Position UnitTrackerClass::unitFlee(Unit unit, Unit currentTarget)
{
	// If either the unit or current target are invalid, return
	if (!unit || !currentTarget)
	{
		return Positions::None;
	}

	// Unit Flee Variables
	double slopeDegree;
	int x, y;
	Position currentTargetPosition = enemyUnits[currentTarget].getPosition();
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
			if (enemyGroundStrengthGrid[x][y] < 1 && (newPosition.getDistance(getNearestChokepoint(currentUnitPosition)->getCenter()) < 128 || (getRegion(currentUnitPosition)) == getRegion(newPosition) && !isThisACorner(newPosition)) && Broodwar->getUnitsOnTile(TilePosition(x, y)).size() < 2)
			{
				return newPosition;

				// Corner checking is the same as walkable check (larger area)
				//for (int i = 0; i <= 1; i++)
				//{
				//	for (int j = 0; j <= 1; j++)
				//	{
				//		// Not a fully functional walkable check -- IMPLEMENTING
				//		if (Broodwar->isWalkable(((x * 4) + i), ((y * 4) + j)))
				//		{
				//			return newPosition + Position(i * 8, j * 8);
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

void UnitTrackerClass::storeEnemyUnit(Unit unit, map<Unit, UnitInfoClass>& enemyUnits)
{
	// Create new unit
	if (enemyUnits.find(unit) == enemyUnits.end())
	{
		UnitInfoClass newUnit(unit->getType(), unit->getPosition(), unitGetVisibleStrength(unit), unitGetStrength(unit->getType()), unitGetTrueRange(unit->getType(), Broodwar->enemy()), unit->getLastCommand().getType(), 0, 0, 0, nullptr);
		enemyUnits[unit] = newUnit;
	}
	// Update unit
	else
	{
		enemyUnits[unit].setUnitType(unit->getType());
		enemyUnits[unit].setPosition(unit->getPosition());
		enemyUnits[unit].setStrength(unitGetVisibleStrength(unit));
		enemyUnits[unit].setMaxStrength(unitGetStrength(unit->getType()));
		enemyUnits[unit].setRange(unitGetTrueRange(unit->getType(), Broodwar->enemy()));
	}

	if ((unit->isCloaked() || unit->isBurrowed()) && !unit->isDetected())
	{
		enemyUnits[unit].setStrength(unitGetStrength(unit->getType()));
	}
	return;
}
void UnitTrackerClass::storeAllyUnit(Unit unit, map<Unit, UnitInfoClass>& allyUnits)
{
	// Create new unit
	if (allyUnits.find(unit) == allyUnits.end())
	{
		UnitInfoClass newUnit(unit->getType(), unit->getPosition(), unitGetVisibleStrength(unit), unitGetStrength(unit->getType()), unitGetTrueRange(unit->getType(), Broodwar->enemy()), unit->getLastCommand().getType(), 0, 0, 0, nullptr);
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

//#pragma region SpecialUnits
//
//void shuttleManager(Unit unit)
//{
//	// Reaver target
//	Unit target = allyUnits[unit].getTarget();
//	UnitInfoClass targetInfo = allyUnits[unit];
//
//	// Check for valid target
//	if (!target)
//	{
//		return;
//	}
//
//	// To implement:
//	// If no targets and within a certain distance of an enemy base, go harass
//	// If Reaver is attacking, patrol
//	// If no units nearby, move to a strong position (based on heatmap strength? ally clusters?)
//
//	// Get local strategy
//	unitGetLocalStrategy(unit, target);
//	// If Reaver has cooldown on attack or strategy is retreat, pickup
//	if (target->isLoaded())
//	{
//		// If Reaver has no cooldown and strategy is attack, drop
//		if (target->getGroundWeaponCooldown() == 0 && targetInfo.getStrategy() == 1 && target->getUnitsInRadius(256, Filter::IsEnemy).size() > 0)
//		{
//			unit->unload(target);
//		}
//		
//		else
//		{
//			unit->move(supportPosition);
//		}
//		
//	}
//
//	else
//	{
//		if (target->getGroundWeaponCooldown() > 0 || targetInfo.getStrategy() == 0 || target->getUnitsInRadius(256, Filter::IsEnemy).size() == 0)
//		{
//			unit->load(target);
//		}
//	}
//	
//
//	// Check tile for air threat
//	// Check enemies around that Reaver can hit
//	// If no air threat, higher Local and Reaver cooldown ready, drop Reaver
//}
//
//void shuttleHarass(Unit unit)
//{
//	if (unit->getLoadedUnits().size() > 0 && (shuttleHeatmap[unit->getTilePosition().x][unit->getTilePosition().y] > 2 || shuttleHeatmap[unit->getTilePosition().x][unit->getTilePosition().y] == 0))
//	{
//		unit->move(unitGetPath(unit, TilePosition(playerStartingPosition)));
//	}
//	else
//	{
//		shuttleManager(unit);
//	}
//}
//

//
//	/*bool harassing = false;
//	if (find(reaverID.begin(), reaverID.end(), unit->getID()) == reaverID.end())
//	{
//	reaverID.push_back(unit->getID());
//	}
//	if (find(harassReaverID.begin(), harassReaverID.end(), unit->getID()) != harassReaverID.end())
//	{
//	harassing = true;
//	}
//
//	if ((unitGetLocalStrategy(unit) == 0) || unit->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsFlyer).size() < 1 || Broodwar->getLatencyFrames() + 30 < unit->getGroundWeaponCooldown())
//	{
//	unitGetCommand(unit);
//	// If a shuttle is following a unit, it is empty, see shuttle manager
//	if (find(reaverID.begin(), reaverID.end(), unit->getID()) - reaverID.begin() < (int)shuttleID.size())
//	{
//	unit->rightClick(Broodwar->getUnit(shuttleID.at(find(reaverID.begin(), reaverID.end(), unit->getID()) - reaverID.begin())));
//	}
//	else if (unit->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsFlyer).size() > 0)
//	{
//	unitGetCommand(unit);
//	}
//	else
//	{
//	unitGetCommand(unit);
//	}
//	}
//	else if (unitGetLocalStrategy(unit) == 1)
//	{
//	unitGetCommand(unit);
//	}
//	*/
//}
//
//void observerManager(Unit unit)
//{		
//	// Make sure we don't overwrite commands
//	if (unit->getLastCommandFrame() < Broodwar->getFrameCount())
//	{
//		unit->move(supportPosition);
//	}
//}
//

//}
//
//
//void carrierManager(Unit unit)
//{
//	if (unit->getInterceptorCount() < 8)
//	{
//		unit->train(UnitTypes::Protoss_Interceptor);
//	}
//	else
//	{
//		unitGetCommand(unit);
//	}
//}
//
//void corsairManager(Unit unit)
//{
//	if (unit->getUnitsInRadius(320, Filter::IsEnemy && Filter::IsFlying).size() > 0)
//	{
//		unit->attack(unit->getClosestUnit(Filter::IsEnemy && Filter::IsFlying));
//	}
//	// If on tile of air threat > 0, move
//	// Unit Micro, need air targeting
//	// Need to move after every attack
//	else
//	{
//		unit->move(unitGetPath(unit, TilePosition(playerStartingPosition)));
//	}
//}
//
//#pragma endregion


void UnitTrackerClass::templarManager(Unit unit)
{
	unitGetClusterTarget(unit);
	Unit target = allyUnits[unit].getTarget();
	int stratL = allyUnits[unit].getStrategy();
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

void UnitTrackerClass::reaverManager(Unit unit)
{
	if (unit->getScarabCount() < 10)
	{
		unit->train(UnitTypes::Protoss_Scarab);
	}
	unitGetCommand(unit);
}

void UnitTrackerClass::arbiterManager(Unit unit)
{
	// Find a position that is the highest concentration of units
	unitGetClusterTarget(unit);

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
		Unit target = allyUnits[unit].getTarget();
		if (target)
		{
			unit->useTech(TechTypes::Stasis_Field, target);
		}
	}
}