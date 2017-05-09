#include "UnitManager.h"
#include "TerrainManager.h"
#include "BWTA.h"
#include "GridManager.h"

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

void UnitTrackerClass::update()
{
	// Reset sizes
	for (auto &size : allySizes)
	{
		size.second = 0;
	}
	for (auto &size : enemySizes)
	{
		size.second = 0;
	}
	supply = 0;
	// Store units
	for (auto u : Broodwar->self()->getUnits())
	{		
		if (u->getType() == UnitTypes::Protoss_Scarab)
		{
			continue;
		}
		supply = supply + u->getType().supplyRequired();
		if (u && u->exists() && u->isCompleted() && !u->getType().isWorker() && !u->getType().isBuilding())
		{
			storeAllyUnit(u, allyUnits);			
		}

	}
	for (auto u : Broodwar->enemy()->getUnits())
	{
		if (u && u->exists() && u->isCompleted())
		{
			storeEnemyUnit(u, enemyUnits);
		}
	}	

	// Check for decayed ally units
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
	// Check for decayed enemy units
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
			if (GridTracker::Instance().getEGroundGrid(x,y) < 1 && (newPosition.getDistance(getNearestChokepoint(currentUnitPosition)->getCenter()) < 128 || (getRegion(currentUnitPosition)) == getRegion(newPosition) && !isThisACorner(newPosition)) && Broodwar->getUnitsOnTile(TilePosition(x, y)).size() < 2)
			{
				return newPosition;			
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
	// Update sizes
	enemySizes[unit->getType().size()] += 1;
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
	// Update sizes
	allySizes[unit->getType().size()] += 1;	
	return;
}

void UnitTrackerClass::removeUnit(Unit unit)
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
//x

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
	//unit->move(supportPosition);
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

	if (true)//supportPosition != Positions::None && supportPosition != Positions::Unknown && supportPosition != Positions::Invalid)
	{
		//unit->move(supportPosition);
	}
	else
	{
		unit->move(TerrainTracker::Instance().getPlayerStartingPosition());
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