#include "UnitManager.h"
Unit currentTarget;

int state = 0;
int commands = 0;
bool kite;
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
	if (unit->getUnitsInRadius(1000, Filter::IsEnemy).size() >= 1)
	{
		unitGetTarget(unit);
	}
	else if (unit->isIdle())
	{
		switch (commandManager()){
		case 0:			
/*			if (unit->getDistance(holdingPosition) > 200)
			{
				unit->attack(Position((holdingPosition.x + rand() % 128 + (-64)), (holdingPosition.y + rand() % 128 + (-64))));				
			}		*/	
			// Block choke
			if (unit->getType() == UnitTypes::Protoss_Zealot && dragoonCnt < 3)
			{
				unit->attack(nearestChokepointPosition.at(0));
			}
			else
			{
				unit->attack(Position((holdingPosition.x + rand() % 128 + (-64)), (holdingPosition.y + rand() % 128 + (-64))));
			}
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
	Unit currentTarget = unit->getClosestUnit(Filter::IsEnemy && Filter::IsVisible
		&& (!Filter::IsBuilding || Filter::GetType == UnitTypes::Enum::Protoss_Photon_Cannon
		|| Filter::GetType == UnitTypes::Enum::Terran_Bunker
		|| Filter::GetType == UnitTypes::Enum::Zerg_Sunken_Colony));
	if (currentTarget && unit->getType())
	{
		Position currentTargetPosition = currentTarget->getPosition();
		Position currentPosition = unit->getPosition();
		Position nextPosition = Position(currentPosition * 2 - currentTargetPosition);

		// Some if statements from UAlbertaBot to ensure units don't studder or attempt to change target mid attack
		if (unit->getLastCommandFrame() >= Broodwar->getFrameCount() || unit->isAttackFrame())
		{
			return;
		}	

		if (unit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit->getTarget() == currentTarget)
		{
			return;
		}

		// If our range is higher than theirs, kite
		if (currentTarget->getType().groundWeapon().maxRange() < unit->getType().groundWeapon().maxRange())
		{
			kite = true;
		}
		// If kite is true and target is within half our attack range, move
		// Needs to consider zerg/zealot speed (IMPLEMENTING)
		if (kite && unit->getDistance(currentTarget) * 2 < unit->getType().groundWeapon().maxRange())
		{
			unit->move(nextPosition);
		}
		// If outside threshold, attack
		else if (unit->getType() != UnitTypes::Protoss_Carrier)
		{
			unit->attack(currentTarget);
		}
	}
}


int commandManager()
{
	// Command manager will be based around how much supply the enemy has lost recently versus ours (IMPLEMENTING)
	if (darkTemplarCnt >= 100)
	{
		Broodwar << "Attacking" << std::endl;
		return 1;
	}
	else
	{
		return 0;
	}
}

void shuttleManager(Unit shuttle)
{
	// Tri state shuttle micro (IMPLEMENTING)
	// 0 is load up and attack
	// 1 is load up and retreat
	// 2 is currently attacking
	if (Unit enemyExists = shuttle->getClosestUnit(Filter::IsEnemy && !Filter::IsBuilding && !Filter::IsWorker))
	{
		// Time to load up
		state = 1;
	}
	else if (Unit enemyDoesntExist = shuttle->getClosestUnit(Filter::IsEnemy && (Filter::IsBuilding || Filter::IsWorker)))
	{
		// Currently attacking
		state = 2;
	}
	else
	{
		state = 0;
	}

	if (state == 0 && shuttle->isIdle() && (shuttle->getLoadedUnits()).size() < 4)
	{
		Unit zealot = shuttle->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Zealot);
		shuttle->load(zealot);
	}
	else if (state == 0 && shuttle->isIdle() && (shuttle->getLoadedUnits()).size() == 4)
	{
		shuttle->move(Position(enemyBasePositions.front().x, shuttle->getPosition().y), true);
		shuttle->move(Position(enemyBasePositions.front().x, enemyBasePositions.front().y), true);
		if (shuttle->getPosition().getDistance(enemyBasePositions.front()) < 200)
		{
			shuttle->unloadAll();
		}
	}
}