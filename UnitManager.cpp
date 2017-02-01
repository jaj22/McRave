#include "UnitManager.h"
#include "TargetManager.h"

Unit currentTarget;

int state = 0;
int commands = 0;
bool kite;
bool push;

using namespace BWAPI;

using namespace std;
// Commands: 
// 0 standby
// 1 attack
// 2 regroup
// 3 defend

// Micro Commands
// 0 Kite
// 1 Aggresive

void unitGetCommand(Unit unit)
{
	if (unit->getUnitsInRadius(1280, Filter::IsEnemy).size() >= 1)
	{
		unitGetTarget(unit);
	}
	// If units idle, check current command for regroup or attack order
	else if (unit->isIdle())
	{
		switch (commandManager()){
		case 0:
			if (unit->getDistance(holdingPosition) > 200)
			{
				unit->move(Position((holdingPosition.x + rand() % 128 + (-64)), (holdingPosition.y + rand() % 128 + (-64))));
			}
			break;
		case 1:
			if (enemyBasePositions.size() > 0)
			{
				unit->attack(enemyBasePositions.front());
			}
			else
			{
				unit->attack(unit->getClosestUnit(Filter::IsEnemy));
			}
			break;
			// case 2;
			// Gather all units into a central locations and regroup
		}
	}
}

void unitGetTarget(Unit unit)
{
	if (Broodwar->getLatency() > unit->getGroundWeaponCooldown())
	{
		currentTarget = targetPriority(unit);
	}

	if (currentTarget && unit)
	{
		currentTargetPosition = currentTarget->getPosition();
		// Some if statements to ensure that the target isn't interrupted when attacking
		if (unit->isAttackFrame() || unit->isStartingAttack())
		{
			return;
		}

		if (Broodwar->getLatency() <= unit->getGroundWeaponCooldown())
		{
			return;
		}

		if (unit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit->getTarget() == currentTarget)
		{
			return;
		}

		if (unit->getDistance(unit->getClosestUnit(Filter::IsAlly && Filter::IsBuilding)) > 2560 && commands == 0)
		{
			unit->move(holdingPosition);
		}

		// Some if statements to turn off kiting if unnecessary
		if (Broodwar->self()->weaponMaxRange(unit->getType().groundWeapon()) <= 32 || currentTarget->getType().topSpeed() > unit->getType().topSpeed() || currentTarget->getType().isFlyer())
		{
			kite = false;
		}

		// If our range is higher than theirs, kite
		else if (currentTarget->getType().groundWeapon().maxRange() < unit->getType().groundWeapon().maxRange())
		{
			kite = true;
		}

		if (kite && unit->getDistance(currentTarget) < unit->getType().groundWeapon().maxRange())
		{
			nextPosition = Position((unit->getPosition() * 2) - currentTargetPosition);
			fleePosition = fleeTo(TilePosition(nextPosition));
			if (fleePosition != BWAPI::Positions::None)
			{
				unit->move(fleePosition);
			}
			else
			{
				unit->attack(currentTarget);
			}
		}

		// Else attack
		else if (unit->getType() != UnitTypes::Protoss_Carrier)
		{
			unit->attack(currentTarget);
		}
	}
}

Position fleeTo(TilePosition desiredPosition)
{
	int x = desiredPosition.x;
	int y = desiredPosition.y;
	int length = 1;
	int j = 0;
	bool first = true;
	int dx = 0;
	int dy = 1;
	// Searches in a spiral around the specified tile position (usually a nexus)
	while (length < 50)
	{
		//If we can build here, return this tile position
		if (x >= 0 && x < BWAPI::Broodwar->mapWidth() && y >= 0 && y < BWAPI::Broodwar->mapHeight())
		{
			if (threatArray[x][y] < 1)
			{
				return BWAPI::Position(x*32, y*32);
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
	return BWAPI::Positions::None;
}

int commandManager()
{
	// Command manager will be based around how much supply the enemy has lost recently versus ours (IMPLEMENTING)
	if ((Broodwar->self()->allUnitCount() - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) - Broodwar->self()->allUnitCount(UnitTypes::Buildings)) > 40)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void shuttleManager(Unit unit)
{
	if (unit->getLoadedUnits().size() == 0)
	{
		unit->follow(unit->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Reaver && Filter::IsAlly));
	}
	else if (unit->getLoadedUnits().size() >= 1)
	{
		unit->follow(unit->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Dragoon && Filter::IsAlly));
	}
	Unitset loadedUnits = unit->getLoadedUnits();
	for (Unitset::iterator itr = loadedUnits.begin(); itr != loadedUnits.end(); itr++)
	{
		if (unit->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsFlyer).size() >= 1 && Broodwar->getLatency() > (*itr)->getGroundWeaponCooldown())
		{
			unit->unload(*itr);
		}
	}
}

void observerManager(Unit unit)
{
	if (unit->getLastCommand().getType() != UnitCommandTypes::Follow || unit->isIdle())
	{
		// Store all dragoons on map in a unitset
		Unitset dragoons = unit->getUnitsInRadius(5000, Filter::GetType == UnitTypes::Enum::Protoss_Dragoon);
		// For each dragoon
		for (Unitset::iterator itr = dragoons.begin(); itr != dragoons.end(); itr++)
		{
			// If there is NOT an observer within 200 pixels of the dragoon
			if ((*itr)->getClosestUnit(Filter::GetType == UnitTypes::Enum::Protoss_Observer)->getDistance((*itr)) >= 320)
			{
				currentTarget = (*itr);
				break;
			}
		}
		// Then follow that dragoon if exists
		if (currentTarget)
		{
			unit->follow(currentTarget);
		}
	}
}

void reaverManager(Unit unit)
{
	if (unit->getScarabCount() < 10)
	{
		unit->train(UnitTypes::Protoss_Scarab);
	}
	if (unit->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsFlyer).size() < 1 && !unit->isAttackFrame())
	{
		// If a shuttle is following a unit, it is empty, see shuttle manager
		unit->rightClick(unit->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Shuttle && Filter::IsFollowing));
	}
	else
	{
		unitGetTarget(unit);
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
