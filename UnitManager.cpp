#include "UnitManager.h"
#include "TerrainManager.h"
#include "BWTA.h"
#include "GridManager.h"
#include "SpecialUnitManager.h"
#include "ProbeManager.h"
#include "BuildingManager.h"

bool stimResearched;

void UnitTrackerClass::update()
{
	storeUnits();
	removeUnits();
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
		// Add supply of this unit
		if (u->getType().supplyRequired() > 0)
		{
			supply = supply + u->getType().supplyRequired();
		}
		// Don't want to store scarabs or units that don't exist or aren't completed
		if (u->getType() == UnitTypes::Protoss_Scarab || !u || !u->exists() || !u->isCompleted())
		{
			continue;
		}
		// Store Probes
		if (u->getType() == UnitTypes::Protoss_Probe)
		{
			ProbeTracker::Instance().storeProbe(u);
		}
		// Store Special Units
		else if (u->getType() == UnitTypes::Protoss_Observer || u->getType() == UnitTypes::Protoss_High_Templar || u->getType() == UnitTypes::Protoss_Arbiter)
		{
			SpecialUnitTracker::Instance().storeUnit(u);
		}
		else if (u->getType().isBuilding())
		{
			BuildingTracker::Instance().storeBuilding(u);
		}
		// Store the rest
		else
		{
			storeAllyUnit(u);
		}
	}

	// For all enemy units
	for (auto player : Broodwar->enemies())
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

void UnitTrackerClass::removeUnits()
{
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

double unitGetStrength(UnitType unitType)
{
	// Some hardcoded values that don't have attacks but should still be considered for strength
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
	if (unitType == UnitTypes::Protoss_Reaver || unitType == UnitTypes::Protoss_Carrier)
	{
		return 50.0;
	}
	if (unitType == UnitTypes::Protoss_High_Templar)
	{
		return 20.0;
	}
	if (unitType == UnitTypes::Protoss_Scarab || unitType == UnitTypes::Terran_Vulture_Spider_Mine || unitType == UnitTypes::Zerg_Egg || unitType == UnitTypes::Zerg_Larva || unitType == UnitTypes::Protoss_Interceptor)
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
		if (unitType == UnitTypes::Protoss_Zealot || unitType == UnitTypes::Terran_Firebat)
		{
			damage = damage * 2.0;
		}

		// Check for Zergling attack speed upgrade
		if (unitType == UnitTypes::Zerg_Zergling && Broodwar->enemy()->getUpgradeLevel(UpgradeTypes::Adrenal_Glands))
		{
			damage = damage * 1.33;
		}

		// Hp, assume half strength for shields, they're not very strong in most matchups
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
	return 1.0;
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

	// Make units under an Arbiter feel stronger
	if (unit->getPlayer() == Broodwar->self() && unit->isCloaked() && !unit->isDetected())
	{
		return 4.0 * hp * unitGetStrength(unit->getType());
	}
	return hp * unitGetStrength(unit->getType());
}
double unitGetTrueGroundRange(UnitType unitType, Player who)
{
	// Ranged upgrade check for Dragoons, Marines, Hydralisks and Bunkers

	if (unitType == UnitTypes::Protoss_Dragoon && who->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
	{
		return 192.0;
	}
	else if ((unitType == UnitTypes::Terran_Marine && who->getUpgradeLevel(UpgradeTypes::U_238_Shells)) || (unitType == UnitTypes::Zerg_Hydralisk && who->getUpgradeLevel(UpgradeTypes::Grooved_Spines)))
	{
		return 160.0;
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

	// Correct range of Reavers
	else if (unitType == UnitTypes::Protoss_Reaver)
	{
		return 256.0;
	}
	return double(unitType.groundWeapon().maxRange());
}
double unitGetTrueAirRange(UnitType unitType, Player who)
{
	if (unitType == UnitTypes::Protoss_Dragoon && who->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
	{
		return 192.0;
	}
	else if ((unitType == UnitTypes::Terran_Marine && who->getUpgradeLevel(UpgradeTypes::U_238_Shells)) || (unitType == UnitTypes::Zerg_Hydralisk && who->getUpgradeLevel(UpgradeTypes::Grooved_Spines)))
	{
		return 160.0;
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
	else if (unitType == UnitTypes::Terran_Goliath && who->getUpgradeLevel(UpgradeTypes::Charon_Boosters))
	{
		return 256.0;
	}
	return double(unitType.airWeapon().maxRange());
}
double unitGetPriority(UnitType unitType)
{
	// Low strength units with high threat	
	if (unitType == UnitTypes::Protoss_Arbiter || unitType == UnitTypes::Protoss_Observer || unitType == UnitTypes::Protoss_Shuttle || unitType == UnitTypes::Terran_Science_Vessel || unitType == UnitTypes::Terran_Dropship || unitType == UnitTypes::Terran_Vulture_Spider_Mine)
	{
		return 50.0;
	}
	// Reduce the value of a bunker so units don't only target bunkers, since they can be repaired easily
	if (unitType == UnitTypes::Terran_Bunker)
	{
		return 2.5;
	}
	else
	{
		return unitGetStrength(unitType);
	}
}
double unitGetTrueSpeed(UnitType unitType, Player who)
{
	double speed = unitType.topSpeed();

	// Adjust based on speed upgrades for Zerglings, Hydralisks, Ultralisks, Shuttle, Observer, Zealot, Vulture of 50%
	if ((unitType == UnitTypes::Zerg_Zergling && who->getUpgradeLevel(UpgradeTypes::Metabolic_Boost)) || (unitType == UnitTypes::Zerg_Hydralisk && who->getUpgradeLevel(UpgradeTypes::Muscular_Augments)) || (unitType == UnitTypes::Zerg_Ultralisk && who->getUpgradeLevel(UpgradeTypes::Anabolic_Synthesis)) || (unitType == UnitTypes::Protoss_Shuttle && who->getUpgradeLevel(UpgradeTypes::Gravitic_Drive)) || (unitType == UnitTypes::Protoss_Observer && who->getUpgradeLevel(UpgradeTypes::Gravitic_Boosters)) || (unitType == UnitTypes::Protoss_Zealot && who->getUpgradeLevel(UpgradeTypes::Leg_Enhancements)) || (unitType == UnitTypes::Terran_Vulture && who->getUpgradeLevel(UpgradeTypes::Ion_Thrusters)))
	{
		speed = speed * 1.5;
	}
	else if (unitType == UnitTypes::Zerg_Overlord && who->getUpgradeLevel(UpgradeTypes::Pneumatized_Carapace))
	{
		speed = speed * 4.0;
	}
	else if (unitType == UnitTypes::Protoss_Scout && who->getUpgradeLevel(UpgradeTypes::Muscular_Augments))
	{
		speed = speed * 1.33;
	}
	return speed;
}

WalkPosition UnitTrackerClass::getMiniTile(Unit unit)
{
	int x = unit->getPosition().x;
	int y = unit->getPosition().y;

	// If it's a unit, we want to find the closest mini tile with the highest resolution (actual pixel width/height)
	if (!unit->getType().isBuilding())
	{
		int mini_x = int((x - x % 8 - (0.5*unit->getType().width())) / 8);
		int mini_y = int((y - y % 8 - (0.5*unit->getType().height())) / 8);
		return WalkPosition(mini_x, mini_y);
	}
	// For buildings, we want the actual tile size resolution (convert half the tile size to pixels by 0.5*tile*32 = 16.0)
	else
	{
		int mini_x = int((x - (16.0*unit->getType().tileWidth())) / 8);
		int mini_y = int((y - (16.0*unit->getType().tileHeight())) / 8);
		return WalkPosition(mini_x, mini_y);
	}
	return WalkPositions::None;
}
set<WalkPosition> UnitTrackerClass::getMiniTilesUnderUnit(Unit unit)
{
	int x = allyUnits[unit].getMiniTile().x;
	int y = allyUnits[unit].getMiniTile().y;
	int width = allyUnits[unit].getType().tileWidth() * 4;
	int height = allyUnits[unit].getType().tileHeight() * 4;
	set<WalkPosition> returnValues;

	for (int i = x; i <= x + width; i++)
	{
		for (int j = y; j <= y + height; j++)
		{
			returnValues.emplace(WalkPosition(i, j));
		}
	}
	return returnValues;
}

void UnitTrackerClass::storeEnemyUnit(Unit unit)
{
	// Update units
	enemyUnits[unit].setUnitType(unit->getType());
	enemyUnits[unit].setPosition(unit->getPosition());
	enemyUnits[unit].setStrength(unitGetVisibleStrength(unit));
	enemyUnits[unit].setMaxStrength(unitGetStrength(unit->getType()));
	enemyUnits[unit].setGroundRange(unitGetTrueGroundRange(unit->getType(), Broodwar->self()));
	enemyUnits[unit].setAirRange(unitGetTrueAirRange(unit->getType(), Broodwar->self()));
	enemyUnits[unit].setSpeed(unitGetTrueSpeed(unit->getType(), Broodwar->self()));
	enemyUnits[unit].setPriority(unitGetPriority(unit->getType()));
	enemyUnits[unit].setCommand(unit->getLastCommand().getType());
	enemyUnits[unit].setMiniTile(getMiniTile(unit));

	// Update sizes
	enemySizes[unit->getType().size()] += 1;
	return;
}

void UnitTrackerClass::storeAllyUnit(Unit unit)
{
	// Update units
	allyUnits[unit].setUnitType(unit->getType());
	allyUnits[unit].setPosition(unit->getPosition());
	allyUnits[unit].setStrength(unitGetVisibleStrength(unit));
	allyUnits[unit].setMaxStrength(unitGetStrength(unit->getType()));
	allyUnits[unit].setGroundRange(unitGetTrueGroundRange(unit->getType(), Broodwar->self()));
	allyUnits[unit].setAirRange(unitGetTrueAirRange(unit->getType(), Broodwar->self()));
	allyUnits[unit].setSpeed(unitGetTrueSpeed(unit->getType(), Broodwar->self()));
	allyUnits[unit].setPriority(unitGetPriority(unit->getType()));
	allyUnits[unit].setCommand(unit->getLastCommand().getType());
	allyUnits[unit].setMiniTile(getMiniTile(unit));

	allySizes[unit->getType().size()] += 1;
	return;
}

void UnitTrackerClass::decayUnit(Unit unit)
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





