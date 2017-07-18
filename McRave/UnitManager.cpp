#include "McRave.h"

void UnitTrackerClass::update()
{
	Display().startClock();
	updateAliveUnits();
	updateDeadUnits();
	updateGlobalCalculations();
	Display().performanceTest(__FUNCTION__);
	return;
}

void UnitTrackerClass::onUnitCreate(Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{
		// Store supply if it costs supply
		if (unit->getType().supplyRequired() > 0)
		{
			supply += unit->getType().supplyRequired();
		}

		// Store Buildings, Bases, Pylons
		if (unit->getType().isResourceDepot())
		{
			Bases().storeBase(unit);
			Buildings().storeBuilding(unit);
		}
		else if (unit->getType() == UnitTypes::Protoss_Pylon)
		{
			Pylons().storePylon(unit);
			Buildings().storeBuilding(unit);
		}
		else if (unit->getType().isBuilding())
		{
			Buildings().storeBuilding(unit);
		}
	}
}

void UnitTrackerClass::onUnitComplete(Unit unit)
{
	// Don't need to store Scarabs
	if (unit->getType() == UnitTypes::Protoss_Scarab)
	{
		return;
	}

	if (unit->getPlayer() == Broodwar->self())
	{
		// Store Workers, Units, Bases(update base grid?)
		if (unit->getType().isWorker())
		{
			Workers().storeWorker(unit);
		}

		if (unit->getType() == UnitTypes::Protoss_Observer || unit->getType() == UnitTypes::Protoss_Reaver || unit->getType() == UnitTypes::Protoss_High_Templar || unit->getType() == UnitTypes::Protoss_Arbiter)
		{
			SpecialUnits().storeUnit(unit);
		}
		else if (unit->getType() == UnitTypes::Protoss_Shuttle)
		{
			Transport().storeUnit(unit);
		}

		if (!unit->getType().isWorker() && !unit->getType().isBuilding())
		{
			storeAlly(unit);
		}
	}
	else if (unit->getType().isResourceContainer())
	{
		Resources().storeResource(unit);
	}
	else if (unit->getPlayer()->isEnemy(Broodwar->self()))
	{
		storeEnemy(unit);
	}
}

void UnitTrackerClass::onUnitMorph(Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{

	}
	else if (unit->getPlayer()->isEnemy(Broodwar->self()))
	{
		enemyUnits[unit].setUnitType(unit->getType());
	}
}

void UnitTrackerClass::updateAliveUnits()
{
	// Update Ally Units
	for (auto& ally : allyUnits)
	{
		if (ally.second.getDeadFrame() == 0)
		{
			updateAlly(ally.second);
		}
	}

	// Update Enemy Units
	for (auto& enemy : enemyUnits)
	{
		if (enemy.second.unit() && enemy.second.unit()->exists())
		{
			updateEnemy(enemy.second);
		}
	}

	// TESTING -- Calculate how a unit is performing
	for (auto& bullet : Broodwar->getBullets())
	{
		if (bullet->exists() && bullet->getSource() && bullet->getSource()->exists() && bullet->getTarget() && bullet->getTarget()->exists())
		{
			if (bullet->getSource()->getPlayer() == Broodwar->self() && myBullets.find(bullet) == myBullets.end())
			{
				myBullets.emplace(bullet);
				double typeMod = 1.0;

				if (!bullet->getTarget()->getType().isFlyer())
				{
					if (bullet->getSource()->getType().groundWeapon().damageType() == DamageTypes::Explosive)
					{
						if (bullet->getTarget()->getType().size() == UnitSizeTypes::Small)
						{
							typeMod = 0.5;
						}
						if (bullet->getTarget()->getType().size() == UnitSizeTypes::Medium)
						{
							typeMod = 0.75;
						}
					}
					if (bullet->getSource()->getType().groundWeapon().damageType() == DamageTypes::Concussive)
					{
						if (bullet->getTarget()->getType().size() == UnitSizeTypes::Large)
						{
							typeMod = 0.25;
						}
						if (bullet->getTarget()->getType().size() == UnitSizeTypes::Medium)
						{
							typeMod = 0.5;
						}
					}

					unitPerformance[bullet->getSource()->getType()] += double(bullet->getSource()->getType().groundWeapon().damageAmount()) * typeMod;
				}
			}
			else
			{
				unitPerformance[bullet->getSource()->getType()] += double(bullet->getSource()->getType().airWeapon().damageAmount());
			}
		}
	}
}

void UnitTrackerClass::updateDeadUnits()
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

void UnitTrackerClass::storeEnemy(Unit unit)
{
	enemyUnits[unit].setUnit(unit);
	enemySizes[unit->getType().size()] += 1;
	return;
}

void UnitTrackerClass::updateEnemy(UnitInfo& unit)
{
	// Update units
	auto t = unit.unit()->getType();
	auto p = unit.unit()->getPlayer();

	// Update information
	unit.setUnitType(t);
	unit.setPosition(unit.unit()->getPosition());
	unit.setWalkPosition(Util().getWalkPosition(unit.unit()));
	unit.setTilePosition(unit.unit()->getTilePosition());
	unit.setPlayer(unit.unit()->getPlayer());

	// Update statistics
	unit.setPercentHealth(Util().getPercentHealth(unit));
	unit.setGroundRange(Util().getTrueRange(t, p));
	unit.setAirRange(Util().getTrueAirRange(t, p));
	unit.setGroundDamage(Util().getTrueGroundDamage(t, p));
	unit.setAirDamage(Util().getTrueAirDamage(t, p));
	unit.setSpeed(Util().getTrueSpeed(t, p));
	unit.setMinStopFrame(Util().getMinStopFrame(t));

	// Update sizes and strength
	unit.setVisibleGroundStrength(Util().getVisibleGroundStrength(unit, p));
	unit.setMaxGroundStrength(Util().getMaxGroundStrength(unit, p));
	unit.setVisibleAirStrength(Util().getVisibleAirStrength(unit, p));
	unit.setMaxAirStrength(Util().getMaxAirStrength(unit, p));
	unit.setPriority(Util().getPriority(unit, p));
	return;
}

void UnitTrackerClass::storeAlly(Unit unit)
{
	allyUnits[unit].setUnit(unit);
	allySizes[unit->getType().size()] += 1;
	return;
}

void UnitTrackerClass::updateAlly(UnitInfo& unit)
{
	auto t = unit.unit()->getType();
	auto p = unit.unit()->getPlayer();

	// Update information
	unit.setUnitType(t);
	unit.setPosition(unit.unit()->getPosition());
	unit.setTilePosition(unit.unit()->getTilePosition());
	unit.setWalkPosition(Util().getWalkPosition(unit.unit()));
	unit.setPlayer(unit.unit()->getPlayer());

	// Update statistics
	unit.setPercentHealth(Util().getPercentHealth(unit));
	unit.setGroundRange(Util().getTrueRange(t, p));
	unit.setAirRange(Util().getTrueAirRange(t, p));
	unit.setGroundDamage(Util().getTrueGroundDamage(t, p));
	unit.setAirDamage(Util().getTrueAirDamage(t, p));
	unit.setSpeed(Util().getTrueSpeed(t, p));
	unit.setMinStopFrame(Util().getMinStopFrame(t));

	// Update sizes and strength
	unit.setVisibleGroundStrength(Util().getVisibleGroundStrength(unit, p));
	unit.setMaxGroundStrength(Util().getMaxGroundStrength(unit, p));
	unit.setVisibleAirStrength(Util().getVisibleAirStrength(unit, p));
	unit.setMaxAirStrength(Util().getMaxAirStrength(unit, p));
	unit.setPriority(Util().getPriority(unit, p));

	if (unit.unit() && unit.unit()->getLastCommand().getTargetPosition().isValid())
	{
		unit.setTargetPosition(unit.unit()->getLastCommand().getTargetPosition());
	}

	// Update calculations
	unit.setTarget(Targets().getTarget(unit));
	getLocalCalculation(unit);
	return;
}

void UnitTrackerClass::removeUnit(Unit unit)
{
	if (allyUnits.find(unit) != allyUnits.end())
	{
		allyUnits[unit].setDeadFrame(Broodwar->getFrameCount());
		allySizes[unit->getType().size()] -= 1;
	}
	else if (enemyUnits.find(unit) != enemyUnits.end())
	{
		enemyUnits[unit].setDeadFrame(Broodwar->getFrameCount());
		enemySizes[unit->getType().size()] -= 1;
	}

	if (unit->getPlayer() == Broodwar->self())
	{
		if (unit->getType().isResourceDepot())
		{
			Bases().removeBase(unit);
		}
		else if (unit->getType().isBuilding())
		{
			Buildings().removeBuilding(unit);
		}
		else if (unit->getType().isWorker())
		{
			Workers().removeWorker(unit);
		}
		else if (unit->getType() == UnitTypes::Protoss_Observer || unit->getType() == UnitTypes::Protoss_Reaver || unit->getType() == UnitTypes::Protoss_High_Templar || unit->getType() == UnitTypes::Protoss_Arbiter)
		{
			SpecialUnits().removeUnit(unit);
		}
		else if (unit->getType() == UnitTypes::Protoss_Shuttle)
		{
			Transport().removeUnit(unit);
		}
	}
	if (unit->getType().isResourceContainer())
	{
		Resources().removeResource(unit);
	}

}

void UnitTrackerClass::getLocalCalculation(UnitInfo& unit) // Will eventually be moved to UTIL as a double function, to be accesible to SpecialUnitManager too
{
	// Variables for calculating local strengths
	double enemyLocalGroundStrength = 0.0, allyLocalGroundStrength = 0.0, timeToTarget = 0.0;
	double enemyLocalAirStrength = 0.0, allyLocalAirStrength = 0.0;

	// Reset local
	unit.setGroundLocal(0);

	// Time to reach target
	if (unit.getPosition().getDistance(unit.getTargetPosition()) > unit.getGroundRange() && unit.getSpeed() > 0.0)
	{
		timeToTarget = max(2.0, (unit.getPosition().getDistance(unit.getTargetPosition()) - unit.getGroundRange()) / unit.getSpeed());
	}

	// If a unit is clearly out of range based on current health (keeps healthy units in the front), set as "no local" and skip calculating
	if (unit.getPosition().getDistance(unit.getTargetPosition()) > 640.0 + (64.0 * (1.0 - unit.getPercentHealth())))
	{
		unit.setStrategy(3);
		return;
	}

	// Check every enemy unit being in range of the target
	for (auto &e : enemyUnits)
	{
		UnitInfo enemy = e.second;
		// Ignore workers and stasised units
		if (enemy.getType().isWorker() || (enemy.unit() && enemy.unit()->exists() && enemy.unit()->isStasised()))
		{
			continue;
		}

		// If a unit is within threat range of the target, add to local strength
		if (enemy.getGroundDamage() > 0 && enemy.getPosition().getDistance(unit.getTargetPosition()) <= enemy.getGroundRange() + (enemy.getSpeed() * timeToTarget))
		{
			// If enemy hasn't died, add to enemy. Otherwise, partially add to ally local
			if (enemy.getDeadFrame() == 0)
			{
				enemyLocalGroundStrength += enemy.getVisibleGroundStrength();
			}
			else
			{
				allyLocalGroundStrength += enemy.getMaxGroundStrength() * 1.0 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(enemy.getDeadFrame())));
			}
		}
		if (enemy.getAirDamage() > 0 && enemy.getPosition().getDistance(unit.getTargetPosition()) <= enemy.getAirRange() + (enemy.getSpeed() * timeToTarget))
		{
			// If enemy hasn't died, add to enemy. Otherwise, partially add to ally local
			if (enemy.getDeadFrame() == 0)
			{
				enemyLocalAirStrength += enemy.getVisibleAirStrength();
			}
			else
			{
				allyLocalAirStrength += enemy.getMaxAirStrength() * 1.0 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(enemy.getDeadFrame())));
			}
		}
	}

	// Check every ally being in range of the target
	for (auto &a : allyUnits)
	{
		UnitInfo ally = a.second;
		// Ignore workers and buildings
		if (ally.getType().isWorker() || ally.getType().isBuilding())
		{
			continue;
		}

		// If a unit is within threat range of the ally unit, add to local strength
		if (ally.getGroundDamage() > 0 && ally.getPosition().getDistance(unit.getPosition()) <= ally.getGroundRange() + (ally.getSpeed() * timeToTarget))
		{
			// If ally hasn't died, add to ally. Otherwise, partially add to enemy local
			if (ally.getDeadFrame() == 0)
			{
				allyLocalGroundStrength += ally.getVisibleGroundStrength();
			}
			else
			{
				enemyLocalGroundStrength += ally.getMaxGroundStrength() * 1.0 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(ally.getDeadFrame())));
			}
		}
		if (ally.getAirDamage() > 0 && ally.getPosition().getDistance(unit.getPosition()) <= ally.getAirRange() + (ally.getSpeed() * timeToTarget))
		{
			// If enemy hasn't died, add to enemy. Otherwise, partially add to ally local
			if (ally.getDeadFrame() == 0)
			{
				allyLocalAirStrength += ally.getVisibleAirStrength();
			}
			else
			{
				enemyLocalAirStrength += ally.getMaxAirStrength() * 1.0 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(ally.getDeadFrame())));
			}
		}
	}

	// If a recall happened recently, cause units to enrage
	if (Broodwar->getFrameCount() - Strategy().getRecallFrame() < 1000 && Strategy().getRecallFrame() > 0)
	{
		allyLocalAirStrength = allyLocalAirStrength * 20.0;
		allyLocalGroundStrength = allyLocalGroundStrength * 20.0;
	}

	// Store the difference of strengths 
	unit.setGroundLocal(allyLocalGroundStrength - enemyLocalGroundStrength);
	unit.setAirLocal(allyLocalAirStrength - enemyLocalAirStrength);

	// Specific High Templar strategy
	if (unit.getType() == UnitTypes::Protoss_High_Templar)
	{
		if (unit.unit()->getEnergy() < 75)
		{
			unit.setStrategy(0);
			return;
		}
	}

	// Specific Medic strategy
	if (unit.getType() == UnitTypes::Terran_Medic)
	{
		if (unit.unit()->getEnergy() <= 0)
		{
			unit.setStrategy(0);
			return;
		}
	}

	// Specific ground unit strategy
	if (!unit.getType().isFlyer())
	{
		// Specific melee strategy
		if (unit.getGroundRange() <= 32)
		{
			// Force to stay on tanks
			if (unit.getTarget() && unit.getTarget()->exists())
			{
				if ((unit.getTarget()->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || unit.getTarget()->getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && unit.getPosition().getDistance(unit.getTargetPosition()) < 128)
				{
					unit.setStrategy(1);
					return;
				}

				// Avoid attacking mines
				if (unit.getTarget()->getType() == UnitTypes::Terran_Vulture_Spider_Mine)
				{
					unit.setStrategy(0);
					return;
				}
			}

			// Check if we are still in defensive mode
			if (globalStrategy == 2)
			{
				// If against rush and not ready to wall up, fight in mineral line
				if (Strategy().isRush() && !Strategy().isHoldRamp())
				{
					if (unit.getTarget() && unit.getTarget()->exists() && (Grids().getResourceGrid(unit.getTarget()->getTilePosition()) > 0 || Grids().getResourceGrid(unit.getTilePosition()) > 0))
					{
						unit.setStrategy(1);
						return;
					}
					else
					{
						unit.setStrategy(2);
						return;
					}
				}

				// Else hold ramp and attack anything within range
				else if (Strategy().isHoldRamp())
				{
					if (unit.getTarget() && unit.getTarget()->exists() && unit.getPosition().getDistance(unit.getTargetPosition()) < 16 && Terrain().isInAllyTerritory(unit.getTarget()) && Terrain().isInAllyTerritory(unit.unit()) && !unit.getTarget()->getType().isWorker())
					{
						unit.setStrategy(1);
						return;
					}
					else
					{
						unit.setStrategy(2);
						return;
					}
				}
			}
		}

		// Specific ranged strategy
		else if (globalStrategy == 2 && unit.getGroundRange() > 32)
		{
			if (unit.getTarget() && unit.getTarget()->exists() && ((Terrain().isInAllyTerritory(unit.unit()) && unit.getPosition().getDistance(unit.getTargetPosition()) <= unit.getGroundRange()) || (Terrain().isInAllyTerritory(unit.getTarget()) && !unit.getTarget()->getType().isWorker())))
			{
				unit.setStrategy(1);
				return;
			}
			else
			{
				unit.setStrategy(2);
				return;
			}
		}

		// If an enemy is within ally territory, engage
		if (unit.getTarget() && unit.getTarget()->exists() && theMap.GetArea(unit.getTarget()->getTilePosition()) && Terrain().isInAllyTerritory(unit.getTarget()))
		{
			unit.setStrategy(1);
			return;
		}

		// If a Reaver is in range of something, engage it
		if (unit.getType() == UnitTypes::Protoss_Reaver && unit.getGroundRange() > unit.getPosition().getDistance(unit.getTargetPosition()))
		{
			unit.setStrategy(1);
			return;
		}
	}

	// If last command was engage
	if (unit.getStrategy() == 1)
	{
		// Latch based system for at least 80% disadvantage to disengage
		if ((!unit.getType().isFlyer() && allyLocalGroundStrength < enemyLocalGroundStrength*0.8) || (unit.getType().isFlyer() && allyLocalAirStrength < enemyLocalAirStrength*0.8))
		{
			unit.setStrategy(0);
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
		if ((!unit.getType().isFlyer() && allyLocalGroundStrength > enemyLocalGroundStrength*1.2) || (unit.getType().isFlyer() && allyLocalAirStrength > enemyLocalAirStrength*1.2))
		{
			unit.setStrategy(1);
			return;
		}
		else
		{
			unit.setStrategy(0);
			return;
		}
	}

	// Disregard local if no target, no recent local calculation and not within ally region
	unit.setStrategy(3);
	return;
}

void UnitTrackerClass::updateGlobalCalculations()
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