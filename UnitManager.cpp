#include "UnitManager.h"
#include "TerrainManager.h"
#include "BWTA.h"
#include "GridManager.h"
#include "SpecialUnitManager.h"

using namespace BWAPI;
using namespace std;
using namespace BWTA;

bool harassing = false;
bool stimResearched = false;

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
		if (u->getType() == UnitTypes::Protoss_Scarab)
		{
			continue;
		}		
		// Store if exists and not building or worker
		if (u && u->exists() && u->isCompleted() && !u->getType().isWorker() && !u->getType().isBuilding())
		{
			storeAllyUnit(u);
		}
		// Add supply of this unit
		if (u->getType().supplyRequired() > 0)
		{
			supply = supply + u->getType().supplyRequired();
		}		
	}

	// For all enemy units
	for (auto &u : Broodwar->enemy()->getUnits())
	{
		// Store if exists
		if (u && u->exists())
		{
			storeEnemyUnit(u);
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
double unitGetTrueRange(UnitType unitType, Player who)
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
WalkPosition getMiniTile(Unit unit)
{
	int x = unit->getPosition().x;
	int y = unit->getPosition().y;

	// We want to fight the closest mini tile without using BWAPI commands (or else it originates to a tile position, lower resolution)
	int mini_x = (x - x % 8 - (0.5*unit->getType().width())) / 8;
	int mini_y = (y - y % 8 - (0.5*unit->getType().height())) / 8;

	return WalkPosition(mini_x, mini_y);
}
set<WalkPosition> UnitTrackerClass::getMiniTilesUnderUnit(Unit unit)
{
	int x = allyUnits[unit].getMiniTile().x;
	int y = allyUnits[unit].getMiniTile().y;
	int width = allyUnits[unit].getUnitType().width() / 4;
	int height = allyUnits[unit].getUnitType().height() / 4;
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
	// Create new unit
	if (enemyUnits.find(unit) == enemyUnits.end())
	{
		UnitInfoClass newUnit(unit->getType(), unit->getPosition(), unitGetVisibleStrength(unit), unitGetStrength(unit->getType()), unitGetTrueRange(unit->getType(), Broodwar->enemy()), unitGetPriority(unit->getType()), unit->getLastCommand().getType(), 0, 0, 0, nullptr, getMiniTile(unit));
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
		enemyUnits[unit].setMiniTile(getMiniTile(unit));
	}
	if ((unit->isCloaked() || unit->isBurrowed()) && !unit->isDetected())
	{
		enemyUnits[unit].setStrength(unitGetStrength(unit->getType()));
	}	
	// Update sizes
	enemySizes[unit->getType().size()] += 1;
	return;
}

void UnitTrackerClass::storeAllyUnit(Unit unit)
{
	// Create new unit
	if (allyUnits.find(unit) == allyUnits.end())
	{
		UnitInfoClass newUnit(unit->getType(), unit->getPosition(), unitGetVisibleStrength(unit), unitGetStrength(unit->getType()), unitGetTrueRange(unit->getType(), Broodwar->enemy()), unitGetPriority(unit->getType()), unit->getLastCommand().getType(), 0, 0, 0, nullptr, getMiniTile(unit));
		allyUnits[unit] = newUnit;		
	}
	// Update unit
	else
	{
		allyUnits[unit].setUnitType(unit->getType());
		allyUnits[unit].setPosition(unit->getPosition());
		allyUnits[unit].setStrength(unitGetVisibleStrength(unit));
		allyUnits[unit].setMaxStrength(unitGetStrength(unit->getType()));
		allyUnits[unit].setRange(unitGetTrueRange(unit->getType(), Broodwar->self()));
		allyUnits[unit].setCommand(unit->getLastCommand().getType());
		allyUnits[unit].setMiniTile(getMiniTile(unit));
	}
	// Update sizes
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





