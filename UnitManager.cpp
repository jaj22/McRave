#include "McRave.h"

void UnitTrackerClass::update()
{
	clock_t myClock;
	double duration = 0.0;
	myClock = clock();	

	storeUnits();
	removeUnits();

	duration = 1000.0 * double(clock() - myClock) / (double)CLOCKS_PER_SEC;
	//Broodwar->drawTextScreen(200, 100, "Unit Manager: %d ms", duration);
}

void UnitTrackerClass::storeUnits()
{
	// Reset sizes and supply
	for (auto &size : allySizes)
	{
		size.second = 0;
	}
	for (auto &size : enemySizes)
	{
		size.second = 0;
	}
	supply = 0;

	// For all ally units
	for (auto &u : Broodwar->self()->getUnits())
	{
		// Don't want to store scarabs or units that don't exist
		if (u->getType() == UnitTypes::Protoss_Scarab || !u || !u->exists())
		{
			continue;
		}

		// Add supply of this unit
		if (u->getType().supplyRequired() > 0)
		{
			supply = supply + u->getType().supplyRequired();
		}

		// Store buildings even if they're not completed
		if (u->getType().isBuilding())
		{
			Buildings().storeBuilding(u);
			if (u->getType().isResourceDepot())
			{
				Bases().storeBase(u);
			}
			else if (u->getType() == UnitTypes::Protoss_Pylon)
			{
				Pylons().storePylon(u);
			}
		}

		// Don't want to store units that aren't completed
		else if (!u->isCompleted())
		{
			continue;
		}

		// Store workers
		else if (u->getType().isWorker())
		{
			Workers().storeWorker(u);
		}
		// Store Special Units in both
		else if (u->getType() == UnitTypes::Protoss_Observer || u->getType() == UnitTypes::Protoss_Reaver || u->getType() == UnitTypes::Protoss_High_Templar || u->getType() == UnitTypes::Protoss_Arbiter || u->getType() == UnitTypes::Terran_Medic)
		{
			storeAllyUnit(u);
			SpecialUnits().storeUnit(u);
		}
		else if (u->getType() == UnitTypes::Protoss_Shuttle)
		{
			storeAllyUnit(u);
			Transport().storeUnit(u);
		}
		// Store the rest
		else
		{
			storeAllyUnit(u);
		}
	}

	// For all enemy units
	for (auto &player : Broodwar->getPlayers())
	{
		if (player->isEnemy(Broodwar->self()))
		{
			for (auto &u : player->getUnits())
			{
				// Store if exists
				if (u && u->exists())
				{
					storeEnemyUnit(u);
				}
			}
		}
	}
}

void UnitTrackerClass::removeUnits()
{
	// Check for decayed ally units
	for (map<Unit, UnitInfo>::iterator itr = allyUnits.begin(); itr != allyUnits.end();)
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
	for (map<Unit, UnitInfo>::iterator itr = enemyUnits.begin(); itr != enemyUnits.end();)
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

void UnitTrackerClass::storeEnemyUnit(Unit unit)
{
	// Update units
	enemyUnits[unit].setStrength(Util().getVisibleStrength(unit, unit->getPlayer()));
	enemyUnits[unit].setMaxStrength(Util().getStrength(unit->getType(), unit->getPlayer()));
	enemyUnits[unit].setGroundRange(Util().getTrueRange(unit->getType(), unit->getPlayer()));
	enemyUnits[unit].setAirRange(Util().getTrueAirRange(unit->getType(), unit->getPlayer()));
	enemyUnits[unit].setPriority(Util().getPriority(unit->getType(), unit->getPlayer()));
	enemyUnits[unit].setGroundDamage(Util().getTrueGroundDamage(unit->getType(), unit->getPlayer()));
	enemyUnits[unit].setAirDamage(Util().getTrueAirDamage(unit->getType(), unit->getPlayer()));
	enemyUnits[unit].setSpeed(Util().getTrueSpeed(unit->getType(), Broodwar->self()));
	enemyUnits[unit].setMinStopFrame(Util().getMinStopFrame(unit->getType()));

	enemyUnits[unit].setUnit(unit);
	enemyUnits[unit].setUnitType(unit->getType());

	enemyUnits[unit].setPosition(unit->getPosition());
	enemyUnits[unit].setWalkPosition(Util().getWalkPosition(unit));
	enemyUnits[unit].setTilePosition(unit->getTilePosition());

	// Update sizes
	enemySizes[unit->getType().size()] += 1;
	return;
}

void UnitTrackerClass::storeAllyUnit(Unit unit)
{
	// Update units
	allyUnits[unit].setStrength(Util().getVisibleStrength(unit, unit->getPlayer()));
	allyUnits[unit].setMaxStrength(Util().getStrength(unit->getType(), unit->getPlayer()));
	allyUnits[unit].setGroundRange(Util().getTrueRange(unit->getType(), unit->getPlayer()));
	allyUnits[unit].setAirRange(Util().getTrueAirRange(unit->getType(), unit->getPlayer()));
	allyUnits[unit].setPriority(Util().getPriority(unit->getType(), unit->getPlayer()));
	allyUnits[unit].setGroundDamage(Util().getTrueGroundDamage(unit->getType(), unit->getPlayer()));
	allyUnits[unit].setAirDamage(Util().getTrueAirDamage(unit->getType(), unit->getPlayer()));
	allyUnits[unit].setSpeed(Util().getTrueSpeed(unit->getType(), Broodwar->self()));
	allyUnits[unit].setMinStopFrame(Util().getMinStopFrame(unit->getType()));

	allyUnits[unit].setUnit(unit);
	allyUnits[unit].setUnitType(unit->getType());
	allyUnits[unit].setPosition(unit->getPosition());
	allyUnits[unit].setTilePosition(unit->getTilePosition());

	allyUnits[unit].setCommand(unit->getLastCommand().getType());
	allyUnits[unit].setWalkPosition(Util().getWalkPosition(unit));
	allyUnits[unit].setTarget(Targets().getTarget(unit));


	// Update sizes and calculations
	allySizes[unit->getType().size()] += 1;
	getGlobalCalculation(unit, allyUnits[unit].getTarget());
	getLocalCalculation(unit, allyUnits[unit].getTarget());
	return;
}

void UnitTrackerClass::decayUnit(Unit unit)
{
	if (allyUnits.find(unit) != allyUnits.end())
	{
		allyUnits[unit].setDeadFrame(Broodwar->getFrameCount());
	}
	else if (enemyUnits.find(unit) != enemyUnits.end())
	{
		enemyUnits[unit].setDeadFrame(Broodwar->getFrameCount());
	}
}

void UnitTrackerClass::getLocalCalculation(Unit unit, Unit target)
{
	// Variables for calculating local strengths
	double enemyLocalStrength = 0.0, allyLocalStrength = 0.0, thisUnit = 0.0;
	Position targetPosition = allyUnits[unit].getTargetPosition();

	int aLarge = getMySizes()[UnitSizeTypes::Large];
	int aMedium = getMySizes()[UnitSizeTypes::Medium];
	int aSmall = getMySizes()[UnitSizeTypes::Small];

	int eLarge = getEnSizes()[UnitSizeTypes::Large];
	int eMedium = getEnSizes()[UnitSizeTypes::Medium];
	int eSmall = getEnSizes()[UnitSizeTypes::Small];

	// Reset local
	allyUnits[unit].setLocal(0);

	if (unit->getDistance(targetPosition) > 640)
	{
		allyUnits[unit].setStrategy(3);
		return;
	}

	// Check every enemy unit being in range of the target
	for (auto &u : enemyUnits)
	{
		// Ignore workers and stasised units
		if (u.second.getType().isWorker() || (u.first && u.first->exists() && u.first->isStasised()))
		{
			continue;
		}

		// Reset unit strength
		thisUnit = 0.0;
		double threatRange = (u.second.getGroundRange() / 32.0) + u.second.getSpeed() + double(u.second.getType().width()) / 32 + double(allyUnits[target].getType().width()) / 32;

		// If a unit is within range of the target, add to local strength
		if (u.second.getPosition().getDistance(targetPosition) / 32.0 < threatRange)
		{
			if (aLarge > 0 || aMedium > 0 || aSmall > 0)
			{
				// If unit is cloaked or burrowed and not detected, drastically increase strength
				if ((u.first->isCloaked() || u.first->isBurrowed()) && !u.first->isDetected())
				{
					thisUnit = 20 * u.second.getMaxStrength();
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
	for (auto &u : allyUnits)
	{
		// Ignore workers and buildings
		if (u.second.getType().isWorker() || u.second.getType().isBuilding())
		{
			continue;
		}

		// Reset unit strength
		thisUnit = 0.0;
		double threatRange = (u.second.getGroundRange() / 32.0) + u.second.getSpeed() + double(u.second.getType().width()) / 32.0 + double(enemyUnits[target].getType().width()) / 32;

		// If a unit is within the range of the ally unit, add to local strength
		if (u.second.getPosition().getDistance(unit->getPosition()) / 32.0 < threatRange)
		{
			if (eLarge > 0 || eMedium > 0 || eSmall > 0)
			{
				// If shuttle, add units inside
				if (u.second.getType() == UnitTypes::Protoss_Shuttle && u.first->getLoadedUnits().size() > 0)
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
					if (u.second.getType().groundWeapon().damageType() == DamageTypes::Explosive)
					{
						thisUnit = u.second.getStrength() * ((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / (eLarge + eMedium + eSmall);
					}
					else if (u.second.getType().groundWeapon().damageType() == DamageTypes::Concussive)
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

	// Specific High Templar behavior
	if (unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		if (unit->getEnergy() > 75)
		{
			allyUnits[unit].setStrategy(1);
			return;
		}
		else
		{
			allyUnits[unit].setStrategy(0);
			return;
		}
	}

	// Specific Zealot Commands
	if (unit->getType() == UnitTypes::Protoss_Zealot)
	{
		// Force Zealots to stay on Tanks and Reavers to shoot if units are near it
		if (target && target->exists() && (enemyUnits[target].getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || enemyUnits[target].getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && unit->getDistance(targetPosition) < 128)
		{
			allyUnits[unit].setStrategy(1);
			return;
		}

		// Check if we are still in defensive mode
		if (globalStrategy == 2)
		{
			// If against rush and not ready to wall up, fight in mineral line
			if (Strategy().isRush() && !Strategy().isHoldRamp())
			{
				if (target && target->exists() && (Grids().getResourceGrid(target->getTilePosition()) > 0 || Grids().getResourceGrid(unit->getTilePosition()) > 0))
				{
					allyUnits[unit].setStrategy(1);
					return;
				}
				else
				{
					allyUnits[unit].setStrategy(2);
					return;
				}
			}

			// Else hold ramp and attack anything within range
			else if (Strategy().isHoldRamp())
			{
				if (target && target->exists() && theMap.GetArea(target->getTilePosition()) && theMap.GetArea(unit->getTilePosition()) && unit->getDistance(target) < 16 && Terrain().getAllyTerritory().find(theMap.GetArea(target->getTilePosition())->Id()) != Terrain().getAllyTerritory().end() && Terrain().getAllyTerritory().find(theMap.GetArea(target->getTilePosition())->Id()) != Terrain().getAllyTerritory().end() && !target->getType().isWorker())
				{
					allyUnits[unit].setStrategy(1);
					return;
				}
				else
				{
					allyUnits[unit].setStrategy(2);
					return;
				}
			}
		}
	}

	// Specific Dragoon commands for early defense
	if (globalStrategy == 2 && unit->getType() == UnitTypes::Protoss_Dragoon)
	{
		if (target && target->exists() && theMap.GetArea(target->getTilePosition()) && ((Terrain().getAllyTerritory().find(theMap.GetArea(unit->getTilePosition())->Id()) != Terrain().getAllyTerritory().end() && unit->getDistance(target) <= allyUnits[unit].getGroundRange()) || (Terrain().getAllyTerritory().find(theMap.GetArea(target->getTilePosition())->Id()) != Terrain().getAllyTerritory().end() && !target->getType().isWorker())))
		{
			allyUnits[unit].setStrategy(1);
			return;
		}
		else
		{
			allyUnits[unit].setStrategy(2);
			return;
		}
	}

	// If an enemy is within ally territory, engage
	if (target && target->exists() && theMap.GetArea(target->getTilePosition()) && Terrain().getAllyTerritory().find(theMap.GetArea(target->getTilePosition())->Id()) != Terrain().getAllyTerritory().end())
	{
		allyUnits[unit].setStrategy(1);
		return;
	}

	// If a Reaver is in range of something, engage it
	if (unit->getType() == UnitTypes::Protoss_Reaver && allyUnits[unit].getGroundRange() > unit->getDistance(targetPosition))
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
		else
		{
			return;
		}
	}
	// If last command was disengage/no command
	else
	{
		// Latch based system for at least 120% advantage to engage
		if (allyLocalStrength >= enemyLocalStrength*1.2)
		{
			allyUnits[unit].setStrategy(1);
			return;
		}
		else
		{
			allyUnits[unit].setStrategy(0);
			return;
		}
	}
	// Disregard local if no target, no recent local calculation and not within ally region
	allyUnits[unit].setStrategy(3);
	return;
}

void UnitTrackerClass::getGlobalCalculation(Unit unit, Unit target)
{
	if (Broodwar->self()->getRace() == Races::Protoss)
	{
		if (Strategy().isFastExpand())
		{
			globalStrategy = 0;
			return;
		}
		// If Zerg, wait for a larger army before moving out
		if (Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge) == 0)
		{
			globalStrategy = 2;
			return;
		}
		// If Toss, wait for Dragoon range upgrade against rushes
		if (Broodwar->enemy()->getRace() == Races::Protoss && Strategy().isRush() && Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge) == 0)
		{
			globalStrategy = 2;
			return;
		}

		if (Strategy().globalAlly() > Strategy().globalEnemy())
		{
			globalStrategy = 1;
			return;
		}
		else
		{
			// If Terran, contain
			if (Broodwar->enemy()->getRace() == Races::Terran)
			{
				globalStrategy = 1;
				return;
			}
			globalStrategy = 0;
			return;
		}
	}
	else if (Broodwar->self()->getRace() == Races::Terran)
	{
		if (Broodwar->self()->completedUnitCount(UnitTypes::Terran_Medic) >= 2)
		{
			globalStrategy = 1;
			return;
		}
		else
		{
			globalStrategy = 0;
			return;
		}
	}
	globalStrategy = 1;
	return;
}