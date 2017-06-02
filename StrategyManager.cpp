#include "StrategyManager.h"
#include "TerrainManager.h"
#include "UnitManager.h"
#include "GridManager.h"
#include "ProbeManager.h"

void StrategyTrackerClass::update()
{
	updateAlly();
	updateEnemy();
	updateComposition();
}

void StrategyTrackerClass::updateAlly()
{
	// Reset	
	globalAllyStrength = 0.0;

	// Check through all alive units or units dead within 500 frames
	for (auto &u : UnitTracker::Instance().getMyUnits())
	{	
		// If deadframe is 0, unit is alive still
		if (u.second.getDeadFrame() == 0)
		{			
			// Strength based calculations ignore workers and buildings
			if (!u.second.getType().isWorker() && !u.second.getType().isBuilding())
			{
				// Add strength				
				globalAllyStrength += u.second.getStrength();
								
				// Set last command frame
				if (u.first->isAttackFrame())
				{					
					u.second.setLastAttackFrame(Broodwar->getFrameCount());
				}

				// Drawing
				if (u.second.getTargetPosition() != Positions::None && u.second.getPosition() != Positions::None && u.first->getDistance(u.second.getTargetPosition()) < 500)
				{
					Broodwar->drawLineMap(u.second.getPosition(), u.second.getTargetPosition(), Broodwar->self()->getColor());
					Broodwar->drawBoxMap(u.second.getTargetPosition() - Position(4,4), u.second.getTargetPosition() + Position(4, 4), Broodwar->self()->getColor(), true);
				}
				if (u.second.getLocal() < 0)
				{
					Broodwar->drawTextMap(u.second.getPosition() + Position(-8, 8), "%c%.2f", Broodwar->enemy()->getTextColor(), u.second.getLocal());
				}
				else if (u.second.getLocal() > 0)
				{
					Broodwar->drawTextMap(u.second.getPosition() + Position(-8, 8), "%c%.2f", Broodwar->self()->getTextColor(), u.second.getLocal());
				}
				else
				{
					Broodwar->drawTextMap(u.second.getPosition() + Position(-8, 8), "%c%.2f", Text::Default, u.second.getLocal());
				}				
			}
		}
		else
		{
			globalEnemyStrength += u.second.getMaxStrength() * 0.5 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));

			Broodwar->drawTextMap(u.second.getPosition() + Position(-8, 8), "%c%d", Broodwar->self()->getTextColor(), Broodwar->getFrameCount() - u.second.getDeadFrame());

		}
	}
}

void StrategyTrackerClass::updateEnemy()
{
	// Reset	
	globalEnemyStrength = 0.0;

	// Stored enemy units iterator
	for (auto &u : UnitTracker::Instance().getEnUnits())
	{
		// If nullptr, continue
		if (!u.first)
		{
			continue;
		}
		// If deadframe is 0, unit is alive still
		if (u.second.getDeadFrame() == 0)
		{
			if (u.second.getType() == UnitTypes::Protoss_Dark_Templar || u.second.getType() == UnitTypes::Protoss_Citadel_of_Adun || u.second.getType() == UnitTypes::Terran_Wraith || u.second.getType() == UnitTypes::Terran_Ghost || u.second.getType() == UnitTypes::Zerg_Lurker)
			{
				invis = true;
			}

			enemyComposition[u.second.getType()] += 1;

			// If tile is visible but unit is not, remove position
			if (!u.first->exists() && u.second.getPosition() != Positions::None && Broodwar->isVisible(TilePosition(u.second.getPosition())))
			{
				u.second.setPosition(Positions::None);
			}

			// Strength based calculations ignore workers and buildings
			if ((u.second.getType().isBuilding() && u.second.getMaxStrength() > 1.0) || (!u.second.getType().isBuilding() && !u.second.getType().isWorker()) || u.first->exists() && TerrainTracker::Instance().getAllyTerritory().find(getRegion(u.first->getTilePosition())) != TerrainTracker::Instance().getAllyTerritory().end())
			{
				// Add strength				
				globalEnemyStrength += u.second.getStrength();
			}

			// Drawing
			if (u.second.getType().isBuilding())
			{
				Broodwar->drawEllipseMap(u.second.getPosition(), u.second.getType().height() / 2, u.second.getType().height() / 3, Broodwar->enemy()->getColor());
			}
			else
			{
				Broodwar->drawEllipseMap(u.second.getPosition() + Position(0, u.second.getType().height() / 2), u.second.getType().height() / 2, u.second.getType().height() / 3, Broodwar->enemy()->getColor());
			}

			Broodwar->drawTextMap(u.second.getPosition() + Position(-8, -8), "%c%.2f", Broodwar->enemy()->getTextColor(), u.second.getStrength());				
		}

		// If unit is dead
		else if (u.second.getDeadFrame() != 0)
		{
			// Add a portion of the strength to ally strength
			globalAllyStrength += u.second.getMaxStrength() * 1 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));


			Broodwar->drawTextMap(u.second.getPosition() + Position(-8, 8), "%c%d", Broodwar->enemy()->getTextColor(), Broodwar->getFrameCount() - u.second.getDeadFrame());

		}
	}
}

void StrategyTrackerClass::updateComposition()
{
	int offset = 0;
	// Reset unit score
	for (auto &unit : unitScore)
	{
		unit.second = 0;
	}

	for (auto &t : enemyComposition)
	{
		// For each type, add a score to production based on the unit count divided by our current unit count
		updateUnitScore(t.first, t.second);
		if (t.first != UnitTypes::None && t.second > 0.0)
		{
			Broodwar->drawTextScreen(500, 50 + offset, "%s : %d", t.first.toString().c_str(), t.second);
			offset = offset + 10;
		}

		if (t.first == UnitTypes::Protoss_Dark_Templar)
		{

		}

		// If we are being 2 gate rushed, make a shield battery
		if (ProbeTracker::Instance().isScouting() && t.first == UnitTypes::Protoss_Gateway && t.second >= 2 && enemyComposition.find(UnitTypes::Protoss_Assimilator) == enemyComposition.end())
		{			
			battery = true;
		}

		// Force expand based on enemy composition
		if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) < 2)
		{
			if (t.first == UnitTypes::Terran_Bunker || (t.first == UnitTypes::Zerg_Sunken_Colony && t.second >= 2) || (t.first == UnitTypes::Protoss_Photon_Cannon && t.second >= 2))
			{
				fastExpand = true;
			}
		}
		else
		{
			fastExpand = false;
		}
		t.second = 0;
	}
}

void StrategyTrackerClass::updateUnitScore(UnitType unit, int count)
{
	switch (unit)
	{
		if (Broodwar->enemy()->getRace() == Races::Terran)
		{
	case Enum::Terran_Marine:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Medic:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Firebat:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Vulture:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Goliath:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Wraith:
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Science_Vessel:
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Battlecruiser:
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (1.00*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Siege_Tank_Siege_Mode:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.85*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.15*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Terran_Siege_Tank_Tank_Mode:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.85*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.15*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
		}
		if (Broodwar->enemy()->getRace() == Races::Zerg)
		{
	case Enum::Zerg_Zergling:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Hydralisk:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Lurker:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Ultralisk:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Mutalisk:
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Guardian:
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
	case Enum::Zerg_Defiler:
		unitScore[Protoss_Zealot] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Zealot))))));
		unitScore[Protoss_Dragoon] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(Protoss_Dragoon))))));
		break;
		}
		if (Broodwar->enemy()->getRace() == Races::Protoss)
		{

		}
	}
}
