#include "UnitManager.h"
#include "TargetManager.h"

Unit currentTarget;

int state = 0;
int commands = 0;
bool kite;

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
	// If units are targeting, send to targeting function (should be sending to a target priority first, IMPLEMENTING)
	if (unit->getUnitsInRadius(5000, Filter::IsEnemy).size() >= 1)
	{
		unitGetTarget(unit);
	}
	else if (unit->isIdle())
	{
		switch (commandManager()){
		case 0:
			if (unit->getDistance(holdingPosition) > 200)
			{
				unit->attack(Position((holdingPosition.x + rand() % 128 + (-64)), (holdingPosition.y + rand() % 128 + (-64))));
			}
			// Block choke
			break;
		case 1:
			unit->attack(enemyBasePositions.front());
			break;
			// case 2;
			// Gather all units into a central locations and regroup
		}
	}

}

void carrierGetCommand(Unit unit)
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

void unitGetTarget(Unit unit)
{	
	currentTarget = targetPriority(unit);
	if (currentTarget && unit)
	{
		// Some if statements to ensure that the target isn't interrupted when attacking
		if (unit->getLastCommandFrame() >= Broodwar->getFrameCount() || unit->isAttackFrame())
		{
			return;
		}

		if (unit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit->getTarget() == currentTarget)
		{
			return;
		}

		// Some if statements to turn off kiting if unnecessary
		if (unit->getType().groundWeapon().maxRange() <= 32)
		{
			kite = false;
		}

		if (unit->isStuck() || !currentTarget->getType().canAttack())
		{
			kite = false;
		}
		// If our range is higher than theirs, kite
		if (currentTarget->getType().groundWeapon().maxRange() < unit->getType().groundWeapon().maxRange())
		{
			kite = true;
		}
		// If kite is true and target is within half our attack range, move
		// Needs to consider zerg/zealot speed (IMPLEMENTING)
		if (kite && unit->getDistance(currentTarget) < unit->getType().groundWeapon().maxRange())
		{
			// Kiting variables
			currentTargetPosition = currentTarget->getPosition();
			currentPosition = unit->getPosition();

			// Get chokepoints connecting these regions
			vector<Position> backupChokes;
			if (BWTA::getRegion(unit->getPosition()))
			{
				set<BWTA::Chokepoint*> currentChokes = BWTA::getRegion(unit->getPosition())->getChokepoints();
				for (std::set<BWTA::Chokepoint*>::iterator itr = currentChokes.begin(); itr != currentChokes.end(); itr++)
				{
					if (Broodwar->getUnitsInRadius((*itr)->getCenter(), 256, Filter::IsEnemy).size() < 1 && (Broodwar->getClosestUnit((*itr)->getCenter(), Filter::IsAlly)->getDistance((*itr)->getCenter()) > Broodwar->getClosestUnit((*itr)->getCenter(), Filter::IsEnemy)->getDistance((*itr)->getCenter())))
					{
						backupChokes.push_back((*itr)->getCenter());
					}
				}
			}

			//chokepointWrap = BWTA::getNearestChokepoint(unit->getPosition())->getCenter();
			int distanceTo = 1;
			if (backupChokes.size() > 0)
			{
				distanceTo = unit->getDistance(backupChokes.at(0));
			}

			if (distanceTo > 64)
			{
				nextPosition = Position(currentPosition*(1 / distanceTo) + chokepointWrap*(1 - (1 / distanceTo)) - currentTargetPosition);
			}
			else
			{
				nextPosition = Position(currentPosition * 2 - currentTargetPosition);
			}
			unit->move(nextPosition);
		}
		// If outside threshold, attack
		else if (unit->getType() != UnitTypes::Protoss_Carrier)
		{
			unit->attack(currentTarget);
		}
	}
	else if (!currentTarget && unit)
	{
		// It's a building, just attack it
		Unit currentTarget = unit->getClosestUnit(Filter::IsEnemy && Filter::IsVisible);
		if (unit->getLastCommandFrame() >= Broodwar->getFrameCount() || unit->isAttackFrame())
		{
			return;
		}
		if (unit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit->getTarget() == currentTarget)
		{
			return;
		}
		unit->attack(currentTarget);
	}
}


int commandManager()
{
	// Command manager will be based around how much supply the enemy has lost recently versus ours (IMPLEMENTING)
	if (zealotCnt + dragoonCnt + darkTemplarCnt + carrierCnt > 40)
	{
		Broodwar << "Attacking" << std::endl;
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
	if (unit->getUnitsInRadius(256, Filter::IsEnemy).size() >= 1)
	{
		unit->unloadAll();
	}

	//// Tri state shuttle micro (IMPLEMENTING)
	//// 0 is load up and attack
	//// 1 is load up and retreat
	//// 2 is currently attacking
	//if (Unit enemyExists = shuttle->getClosestUnit(Filter::IsEnemy && !Filter::IsBuilding && !Filter::IsWorker))
	//{
	//	// Time to load up
	//	state = 1;
	//}
	//else if (Unit enemyDoesntExist = shuttle->getClosestUnit(Filter::IsEnemy && (Filter::IsBuilding || Filter::IsWorker)))
	//{
	//	// Currently attacking
	//	state = 2;
	//}
	//else
	//{
	//	state = 0;
	//}

	//if (state == 0 && shuttle->isIdle() && (shuttle->getLoadedUnits()).size() < 4)
	//{
	//	Unit zealot = shuttle->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Zealot);
	//	shuttle->load(zealot);
	//}
	//else if (state == 0 && shuttle->isIdle() && (shuttle->getLoadedUnits()).size() == 4)
	//{
	//	shuttle->move(Position(enemyBasePositions.front().x, shuttle->getPosition().y), true);
	//	shuttle->move(Position(enemyBasePositions.front().x, enemyBasePositions.front().y), true);
	//	if (shuttle->getPosition().getDistance(enemyBasePositions.front()) < 200)
	//	{
	//		shuttle->unloadAll();
	//	}
	//}
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
	if (unit->getUnitsInRadius(256, Filter::IsEnemy).size() < 1)
	{
		// If a shuttle is following a unit, it is empty, see shuttle manager
		unit->rightClick(unit->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Shuttle && Filter::IsFollowing));
	}
}