// McRave is made by Christian McCrave
// Twitch nicknamed it McRave \o/
// For any questions, email christianmccrave@gmail.com
// Bot started 01/03/2017

// Includes
#include "Header.h"
#include "Global.h"

// Variables for Global.cpp
Color playerColor;
bool analyzed = false;
bool analysis_just_finished = false;

bool analyzeHome = true;
bool BWTAhandling = false;
int enemyScoutedLast = 0;
namespace { auto & theMap = BWEM::Map::Instance(); }

// Drawing booleans
bool masterDraw = true;
bool calculationDraw = false;

void McRave::onStart()
{
	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Set the command optimization level so that common commands can be grouped and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	theMap.Initialize();
	theMap.EnableAutomaticPathAnalysis();
	bool startingLocationsOK = theMap.FindBasesForStartingLocations();
	assert(startingLocationsOK);

	BWEM::utils::MapPrinter::Initialize(&theMap);
	BWEM::utils::printMap(theMap);      // will print the map into the file <StarCraftFolder>bwapi-data/map.bmp
	BWEM::utils::pathExample(theMap);   // add to the printed map a path between two starting locations

	if (analyzed == false) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
	}
	readMap();
	analyzed = false;
	analysis_just_finished = false;
	playerColor = Broodwar->self()->getColor();
}

void McRave::onEnd(bool isWinner)
{
	// Called when the game ends
	if (isWinner)
	{
		// Log your win here!
	}
}

void McRave::onFrame()
{
	// Test area	

#pragma region Heatmap Manager
	double strongest = 0;
	// For each tile, draw the current threat onto the tile
	for (int x = 0; x <= Broodwar->mapWidth(); x++)
	{
		for (int y = 0; y <= Broodwar->mapHeight(); y++)
		{
			if (enemyHeatmap[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%.2f", enemyHeatmap[x][y]);
			}
			if (shuttleHeatmap[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", shuttleHeatmap[x][y]);
			}
			if (allyHeatmap[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%.2f", allyHeatmap[x][y]);
			}
			if (clusterHeatmap[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", clusterHeatmap[x][y]);
			}
			if (mineralHeatmap[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", mineralHeatmap[x][y]);
			}

			if (((allyHeatmap[x][y] - enemyHeatmap[x][y]) - (Position(x * 32, y * 32).getDistance(enemyStartingPosition) / 320)) > strongest && allyHeatmap[x][y] > 0)
			{
				supportPosition = Position(x * 32, y * 32);
				strongest = (allyHeatmap[x][y] - enemyHeatmap[x][y]) - (Position(x * 32, y * 32).getDistance(enemyStartingPosition) / 320);
			}

			if (Broodwar->isVisible(x, y))
			{
				enemyHeatmap[x][y] = 0;
				allyHeatmap[x][y] = 0;
				airEnemyHeatmap[x][y] = 0;
			}
			// Reset clusters regardless
			clusterHeatmap[x][y] = 0;
		}
	}
	// For each enemy unit, add its attack value to each tile it is in range of
	for (auto u : Broodwar->enemy()->getUnits())
	{
		if (u->getType().groundWeapon().damageAmount() > 0)
		{
			int range;
			// Making sure we properly analyze the threat of melee units without adding range to ranged units
			if (u->getType().groundWeapon().maxRange() < 32)
			{
				range = (u->getType().groundWeapon().maxRange() + 64) / 32;
			}
			else
			{
				range = (u->getType().groundWeapon().maxRange()) / 32;
			}
			// The + 1 is because we need to still check an additional tile
			for (int x = u->getTilePosition().x - range; x <= u->getTilePosition().x + range + 1; x++)
			{
				for (int y = u->getTilePosition().y - range; y <= u->getTilePosition().y + range + 1; y++)
				{
					if (Broodwar->isVisible(x, y) && (u->getDistance(Position((x * 32), (y * 32))) <= (range * 32)) && x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight())
					{
						enemyHeatmap[x][y] += unitGetStrength(u->getType());
					}
				}
			}
		}
		if (u->getType().airWeapon().damageAmount() > 0)
		{
			int range = (u->getType().airWeapon().maxRange()) / 32;
			// The + 1 is because we need to still check an additional tile
			for (int x = u->getTilePosition().x - range; x <= u->getTilePosition().x + range + 1; x++)
			{
				for (int y = u->getTilePosition().y - range; y <= u->getTilePosition().y + range + 1; y++)
				{
					if (Broodwar->isVisible(x, y) && (u->getDistance(Position((x * 32), (y * 32))) <= (range * 32)) && x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight())
					{
						airEnemyHeatmap[x][y] += unitGetAirStrength(u->getType());
					}
				}
			}
		}
		if (!u->getType().isBuilding() && !u->isStasised() && !u->isMaelstrommed())
		{
			// Cluster heatmap for psi/stasis (96x96)			
			for (int x = u->getTilePosition().x - 1; x <= u->getTilePosition().x + 1; x++)
			{
				for (int y = u->getTilePosition().y - 1; y <= u->getTilePosition().y + 1; y++)
				{
					if ((x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight()))
					{
						clusterHeatmap[x][y] += 1;
						if (u->getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode || u->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode)
						{
							tankClusterHeatmap[x][y] += 1;
						}
					}
				}
			}
		}
	}
	for (auto u : Broodwar->self()->getUnits())
	{
		if (u->getType() == UnitTypes::Protoss_Reaver || (u->getType().groundWeapon().damageAmount() > 0 && u->isCompleted() && !u->getType().isWorker() && !u->getType().isBuilding()))
		{
			int range;

			if (u->getType() == UnitTypes::Protoss_Reaver)
			{
				range = 8;
			}
			// Making sure we properly analyze the threat of melee units without adding range to ranged units
			else if (u->getType().groundWeapon().maxRange() < 32)
			{
				range = (u->getType().groundWeapon().maxRange() + 64) / 32;
			}
			else
			{
				range = (u->getType().groundWeapon().maxRange()) / 32;
			}
			// The + 1 is because we need to still check an additional tile
			for (int x = u->getTilePosition().x - range; x <= u->getTilePosition().x + range + 1; x++)
			{
				for (int y = u->getTilePosition().y - range; y <= u->getTilePosition().y + range + 1; y++)
				{
					if (Broodwar->isVisible(x, y) && (u->getDistance(Position((x * 32), (y * 32))) <= (range * 32)) && (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight()))
					{
						allyHeatmap[x][y] += unitGetStrength(u->getType());
					}
				}
			}
		}
	}
	// Mineral grid for not building near minerals
	for (auto r : Broodwar->neutral()->getUnits())
	{
		if (r->getType().isMineralField() || r->getType() == UnitTypes::Resource_Vespene_Geyser)
		{
			for (int x = r->getTilePosition().x - 1; x <= r->getTilePosition().x + r->getType().tileWidth(); x++)
			{
				for (int y = r->getTilePosition().y - 1; y <= r->getTilePosition().y + r->getType().tileHeight(); y++)
				{
					if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight())
					{
						mineralHeatmap[x][y] = 1;
					}
				}
			}
		}
	}
#pragma endregion
#pragma region Territory Manager
	if (!analyzeHome)
	{
		if (territory.size() < 1)
		{
			allyTerritory.push_back(getRegion(playerStartingPosition));
			territory = BWTA::getRegions();
		}
		// In each ally territory there are chokepoints to defend, find and store those chokepoints
		if (allyTerritory.size() != currentSize && allyTerritory.size() > 0)
		{
			// For all regions
			for (auto *region : territory)
			{
				bool merge = false;
				// For each chokepoint
				for (auto Chokepoint : region->getChokepoints())
				{
					if (region == getRegion(playerStartingTilePosition))
					{
						// If chokepoint is really wide, such as Andromeda, this will add the Territory of the natural
						if (Chokepoint->getWidth() > 200 && find(allyTerritory.begin(), allyTerritory.end(), region) == allyTerritory.end())
						{
							allyTerritory.push_back(Chokepoint->getRegions().first);
							allyTerritory.push_back(Chokepoint->getRegions().second);
						}
					}
					// Check if every chokepoint is connected to an ally territory, in which case add this to our territory (connecting regions)
					if (find(allyTerritory.begin(), allyTerritory.end(), Chokepoint->getRegions().first) != allyTerritory.end() || find(allyTerritory.begin(), allyTerritory.end(), Chokepoint->getRegions().second) != allyTerritory.end())
					{
						merge = true;
						if (region->getChokepoints().size() == 1)
						{
							break;
						}
					}
					else
					{
						merge = false;
						break;
					}
				}
				if (merge == true && find(allyTerritory.begin(), allyTerritory.end(), region) == allyTerritory.end())
				{
					allyTerritory.push_back(region);
				}
			}
			defendHere.erase(defendHere.begin(), defendHere.end());
			currentSize = allyTerritory.size();
			// For each region that is ally territory
			for (auto *region : allyTerritory)
			{
				// For each chokepoint of each ally region				
				for (auto Chokepoint : region->getChokepoints())
				{
					// Check if both territories are ally
					if (find(allyTerritory.begin(), allyTerritory.end(), Chokepoint->getRegions().first) != allyTerritory.end() && find(allyTerritory.begin(), allyTerritory.end(), Chokepoint->getRegions().second) != allyTerritory.end())
					{
						// If both are ally, do nothing (we don't need to defend two ally regions)							
					}
					else
					{
						defendHere.push_back(Chokepoint->getCenter());
					}
				}
			}
		}
		// Draw territory boundaries
		for (auto position : defendHere)
		{
			Broodwar->drawCircleMap(position, 80, playerColor);
		}
	}
	for (auto p : defendHere)
	{
		// Remove blocking mineral patches (maps like Destination)
		if (boulders.size() > 0 && Broodwar->getUnitsInRadius(p, 64, Filter::IsMineralField && Filter::Resources == 0).size() > 0)
		{
			defendHere.erase(find(defendHere.begin(), defendHere.end(), p));
		}
	}
#pragma endregion
#pragma region Base Manager	
	// Only do this loop once if map analysis done
	if (analyzed)
	{
		//drawTerrainData();
		if (analyzeHome)
		{
			buildChokeNodes();
			analyzeHome = false;

			// Find player starting position and tile position		
			BaseLocation* playerStartingLocation = getStartLocation(Broodwar->self());
			playerStartingPosition = playerStartingLocation->getPosition();
			playerStartingTilePosition = playerStartingLocation->getTilePosition();
			nextExpansion.push_back(playerStartingTilePosition);
			activeExpansion.push_back(playerStartingTilePosition);
		}
		if (analysis_just_finished)
		{
			map <int, TilePosition> testBases;
			if (enemyBasePositions.size() > 0)
			{
				analysis_just_finished = false;
				for (auto base : getBaseLocations())
				{
					int distances = getGroundDistance2(base->getTilePosition(), playerStartingTilePosition) - getGroundDistance2(base->getTilePosition(), enemyStartingTilePosition);
					if (base->isMineralOnly())
					{
						distances += 1280;
					}
					if (base->getTilePosition() != playerStartingTilePosition && !base->isIsland() && base->getTilePosition() != enemyStartingTilePosition)
					{
						testBases.emplace(distances, base->getTilePosition());
					}
				}

				for (auto base : testBases)
				{
					nextExpansion.push_back(base.second);
					activeExpansion.push_back(base.second);
				}
			}
		}
	}
#pragma endregion
#pragma region HUD
	if (masterDraw)
	{
		int offset = 0;
		for (auto b : buildingDesired)
		{
			if (b.second > 0)
			{
				Broodwar->drawTextScreen(0, offset, "%s : %d", b.first.toString().c_str(), b.second);
				offset = offset + 10;
			}
		}

		// Display some information about our queued resources required for structure building			
		Broodwar->drawTextScreen(200, 0, "Current Strategy: %s", currentStrategy.c_str());

		// Display frame count
		Broodwar->drawTextScreen(200, 10, "%d", Broodwar->getFrameCount());

		// Display global strength calculations	
		Broodwar->drawTextScreen(500, 20, "A: %.2f    E: %.2f", allyStrength, enemyStrength);

		// Display remaining minerals on each mineral patch that is near our Nexus
		for (auto r : mineralMap)
		{
			//Broodwar->drawTextMap(r.first->getPosition(), "%d", r.first->getResources());
			Broodwar->drawTextMap(r.first->getPosition(), "%d", r.second);
		}

		// Show local calculations, targets and radius check
		if (calculationDraw)
		{
			for (auto u : localEnemy)
			{
				if (u.second > 0.0)
				{
					Broodwar->drawTextMap(u.first->getPosition(), "E: %.2f", u.second);
				}
			}
			for (auto u : localAlly)
			{
				if (u.second > 0.0)
				{
					Broodwar->drawTextMap(u.first->getPosition() + Position(0, 10), "A: %.2f", u.second);
				}
			}
			for (auto u : unitRadiusCheck)
			{
				Broodwar->drawCircleMap(u.first->getPosition(), u.second, playerColor);
			}
		}
		for (auto u : unitsCurrentTarget)
		{
			Broodwar->drawLineMap(u.first->getPosition(), u.second, playerColor);
		}

		// Show path numbers
		/*for (auto p : path)
		{
		Broodwar->drawTextMap(Position(p->Center()), "%d", find(path.begin(), path.end(), p) - path.begin());
		}*/

		// Show unit composition information
		offset = 0;
		for (auto t : enemyComposition)
		{
			if (t.first != UnitTypes::None && t.second > 0)
			{
				Broodwar->drawTextScreen(500, 50 + offset, "%s : %d", t.first.toString().c_str(), t.second);
				offset = offset + 10;
			}
		}

		offset = 0;
		for (auto t : unitScore)
		{
			Broodwar->drawTextScreen(500, 200 + offset, "%s : %.2f", t.first.toString().c_str(), t.second);
			offset = offset + 10;
		}

		// Show Probe information
		for (auto p : mineralProbeMap)
		{
			Broodwar->drawLineMap(p.first->getPosition(), p.second.second, playerColor);
		}
		for (auto p : gasProbeMap)
		{
			if (getRegion(p.first->getTilePosition()) == getRegion(p.second->getTilePosition()) && p.first->getOrder() != Orders::WaitForGas)
			{
				Broodwar->drawLineMap(p.first->getPosition(), p.second->getPosition(), playerColor);
			}
		}

		// Show building placements
		for (auto b : queuedBuildings)
		{
			Broodwar->drawBoxMap(Position(b.second.first), Position((b.second.first.x + b.first.tileWidth()) * 32, (b.second.first.y + b.first.tileHeight()) * 32), playerColor);
		}

		// Show support position
		//Broodwar->drawCircleMap(supportPosition, 8, playerColor, true);

		// Show expansions
		if (analyzed)
		{
			for (int i = 0; i <= (int)activeExpansion.size() - 1; i++)
			{
				Broodwar->drawTextMap(activeExpansion.at(i).x * 32, activeExpansion.at(i).y * 32, "Base %d", i, Colors::White);
			}

		}
	}
#pragma endregion
#pragma region Scouting Midgame
	// Scouting if we can't find enemy bases
	if (enemyBasePositions.size() < 1 && Broodwar->getFrameCount() > 10000 && Broodwar->getFrameCount() > enemyScoutedLast + 1000 && Broodwar->getUnitsInRadius(playerStartingPosition, 50000, Filter::IsBuilding && Filter::IsEnemy).size() < 1)
	{
		enemyScoutedLast = Broodwar->getFrameCount();
		Broodwar << "Can't find enemy, scouting out." << endl;
		for (auto base : getBaseLocations())
		{
			if (!Broodwar->isVisible(base->getTilePosition()))
			{
				Broodwar->getClosestUnit(base->getPosition(), Filter::IsAlly && !Filter::IsBuilding && !Filter::IsWorker)->attack(base->getPosition());
			}
		}
	}
#pragma endregion
#pragma region Structure Iterator
	// Get build order based on enemy race(s) and enemy army composition
	getBuildOrder();

	// Reserved minerals for idle buildings, tech and upgrades
	reservedMineral = 0, reservedGas = 0;
	for (auto b : idleBuildings)
	{
		reservedMineral += b.second.mineralPrice();
		reservedGas += b.second.gasPrice();
	}
	for (auto t : idleTech)
	{
		reservedMineral += t.second.mineralPrice();
		reservedGas += t.second.gasPrice();
	}
	for (auto u : idleUpgrade)
	{
		reservedMineral += u.second.mineralPrice();
		reservedGas += u.second.gasPrice();
	}
	// For each building in the protoss race
	for (auto b : buildingDesired)
	{
		// If our visible count is lower than our desired count
		if (b.second > Broodwar->self()->visibleUnitCount(b.first) && queuedBuildings.find(b.first) == queuedBuildings.end())
		{
			// Get a Tile Position and a Builder
			TilePosition here = buildingManager(b.first);
			Unit builder = Broodwar->getClosestUnit(Position(here), Filter::IsAlly && Filter::IsWorker && !Filter::IsCarryingSomething && !Filter::IsGatheringGas);
			// If the Tile Position and Builder are valid
			if (here != TilePositions::None && builder)
			{
				// Queue at this building type a pair of building placement and builder
				queuedBuildings.emplace(b.first, make_pair(here, builder));
			}
		}
	}

	// Queued minerals for buildings needed
	queuedMineral = 0, queuedGas = 0;

	for (auto b : queuedBuildings)
	{
		// If probe died, remove it and the building from the queue so that a new queue can be made -- IMPLEMENTING REPLACE PROBE INSTEAD
		if (!b.second.second->exists())
		{
			queuedBuildings.erase(b.first);
			continue;
		}
		queuedMineral += b.first.mineralPrice();
		queuedGas += b.first.gasPrice();

		// If drawing is on, draw a box around the build position -- MOVE -> drawing section
		if (masterDraw)
		{
			Broodwar->drawLineMap(Position(b.second.first), b.second.second->getPosition(), playerColor);
		}

		// If we issued a command to this Probe already, skip
		if (b.second.second->isConstructing() || b.second.second->getLastCommandFrame() >= Broodwar->getFrameCount() && (b.second.second->getLastCommand().getType() == UnitCommandTypes::Move || b.second.second->getLastCommand().getType() == UnitCommandTypes::Build))
		{
			continue;
		}

		if (!Broodwar->canBuildHere(b.second.first, b.first, b.second.second) && Broodwar->isVisible(b.second.first))
		{
			// If Nexus, check units in rectangle of build position, if no ally units, send observer -- IMPLEMENTING
			queuedBuildings.erase(b.first);
		}

		// If we almost have enough resources, move the Probe to the build position
		if (Broodwar->self()->minerals() >= 0.8*b.first.mineralPrice() && Broodwar->self()->minerals() <= b.first.mineralPrice() && Broodwar->self()->gas() >= 0.8*b.first.gasPrice() && Broodwar->self()->gas() <= b.first.gasPrice() || (b.second.second->getDistance(Position(b.second.first)) > 160 && Broodwar->self()->minerals() >= 0.8*b.first.mineralPrice() && 0.8*Broodwar->self()->gas() >= b.first.gasPrice()))
		{
			b.second.second->move(Position(b.second.first));
			continue;
		}
		// If Probe is not currently returning minerals or constructing, the build position is valid and can afford the building, then proceed with build
		else if (b.second.first != TilePositions::None && Broodwar->self()->minerals() >= b.first.mineralPrice() && Broodwar->self()->gas() >= b.first.gasPrice())
		{
			b.second.second->build(b.first, b.second.first);
			continue;
		}

	}
#pragma endregion
#pragma region Ally Unit Information
	// Check all units for their current health, shields and damage capabilities to compare against enemy
	allyStrength = 0.0;
	outsideBase = false;
	aSmall = 0, aMedium = 0, aLarge = 0;
	for (auto u : Broodwar->self()->getUnits())
	{
		if (u->isCompleted() && !u->getType().isWorker() && !u->getType().isBuilding())
		{
			allyStrength += unitGetVisibleStrength(u);
		}
		// If it's not a worker or building, store the unit size type
		if (!u->getType().isWorker() && !u->getType().isBuilding())
		{
			if (u->getType().size() == UnitSizeTypes::Small)
			{
				aSmall++;
			}
			else if (u->getType().size() == UnitSizeTypes::Medium)
			{
				aMedium++;
			}
			else
			{
				aLarge++;
			}
		}
	}
#pragma endregion
#pragma region Enemy Unit Information
	// Reset unit score
	for (auto &unit : unitScore)
	{
		unit.second = 0;
	}

	// Store enemy composition
	for (auto &t : enemyComposition)
	{
		// For each type, add a score to production based on the unit count divided by our current unit count
		unitScoreUpdate(t.first, t.second);
		if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) < 2)
		{
			if (t.first == UnitTypes::Terran_Bunker)
			{
				forceExpand = 1;
			}
			if (t.first == UnitTypes::Zerg_Sunken_Colony && t.second >= 2)
			{
				forceExpand = 1;
			}
		}
		if (t.first == UnitTypes::Terran_Marine)
		{
			if (t.second >= 4)
			{
				terranBio = true;
			}
			else
			{
				terranBio = false;
			}
		}
		if (t.first == UnitTypes::Zerg_Mutalisk && t.second > 2)
		{
			// Put stuff for cannon defenses
		}
		t.second = 0;
	}

	// Store enemy units
	for (auto u : Broodwar->enemy()->getUnits())
	{
		// Update unit in class
		if (u && u->exists())
		{
			storeEnemyUnit(u, enemyUnits);
		}
	}

	// Enemy unit iterator
	eSmall = 0, eMedium = 0, eLarge = 0;
	enemyStrength = 0.0;
	for (auto &u : enemyUnits)
	{
		// Set unit and get strength of unit
		Unit e = Broodwar->getUnit(u.first);
		double thisUnit;

		// Update strength
		if (e && e->exists())
		{
			thisUnit = unitGetVisibleStrength(e);
			// Geyser morph issue
			if (e->getPlayer() != Broodwar->enemy())
			{
				enemyUnits.erase(u.first);
			}
		}
		else
		{
			thisUnit = unitGetStrength(u.second.getUnitType());
		}


		// If tile is visible but unit is not, remove position
		if (!e->exists() && Broodwar->isVisible(TilePosition(u.second.getPosition())))
		{
			u.second.setPosition(Positions::None);
		}

		// Store enemy unit strength if necessary
		if (thisUnit > 1.0 || (e->exists() && find(allyTerritory.begin(), allyTerritory.end(), getRegion(e->getTilePosition())) != allyTerritory.end()))
		{
			enemyStrength += thisUnit;
			enemyComposition[u.second.getUnitType()] += 1;
			if (masterDraw && calculationDraw)
			{
				Broodwar->drawTextMap(u.second.getPosition(), "%.2f", thisUnit);
			}
		}

		// If the unit is invisible and not detected, store the unit
		if ((e->isCloaked() || e->isBurrowed()) && !e->isDetected())
		{
			thisUnit = unitGetStrength(e->getType());
			invisibleUnits[e] = e->getPosition();
		}

		// Regardless of visibility, if it's not a worker or building, store the unit size type
		if (!u.second.getUnitType().isWorker() && !u.second.getUnitType().isBuilding())
		{
			if (u.second.getUnitType().size() == UnitSizeTypes::Small)
			{
				eSmall++;
			}
			else if (u.second.getUnitType().size() == UnitSizeTypes::Medium)
			{
				eMedium++;
			}
			else
			{
				eLarge++;
			}
		}
	}

	// For each invisible unit, if it's detected, remove it
	for (auto u : invisibleUnits)
	{
		if (u.first && u.first->exists() && (u.first->isCloaked() || u.first->isBurrowed()) && u.first->isDetected())
		{
			invisibleUnits.erase(u.first);
		}
	}
#pragma endregion

#pragma region Unit Iterator
	// If not a latency frame, return to prevent spamming
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;

	// For each Probe mapped to gather minerals
	for (auto u : mineralProbeMap)
	{
		if (u.first && u.first->exists())
		{
			// If idle and carrying minerals, return cargo
			if (u.first->isIdle() && u.first->isCarryingMinerals())
			{
				u.first->returnCargo();
				continue;
			}
			if (u.first->getLastCommandFrame() >= Broodwar->getFrameCount() && (u.first->getLastCommand().getType() == UnitCommandTypes::Move || u.first->getLastCommand().getType() == UnitCommandTypes::Build))
			{
				continue;
			}
			// If not scouting and there's boulders to remove
			if (!scouting && boulders.size() > 0)
			{
				for (auto b : boulders)
				{
					if (!u.first->isGatheringMinerals() && u.first->getDistance(b) < 256)
					{
						u.first->gather(b);
					}
				}
			}

			// If idle and not targeting the mineral field the Probe is mapped to
			if (u.first->isIdle() || (u.first->isGatheringMinerals() && !u.first->isCarryingMinerals() && u.first->getTarget() != u.second.first))
			{
				// If the Probe has a target
				if (u.first->getTarget())
				{
					// If the target has a resource count of 0 (mineral blocking a ramp), let Probe continue mining it
					if (u.first->getTarget()->getResources() == 0)
					{
						continue;
					}
				}
				// If the mineral field is in vision and no target, force to gather from the assigned mineral field
				if (u.second.first->exists())
				{
					u.first->gather(u.second.first);
					continue;
				}
			}
		}
	}
	// For each Probe mapped to gather gas
	for (auto u : gasProbeMap)
	{
		if (u.first && u.first->exists())
		{
			// If idle and carrying gas, return cargo
			if (u.first->isIdle() && u.first->isCarryingGas())
			{
				u.first->returnCargo();
				continue;
			}
			// If idle, gather gas
			else if (u.first->isIdle())
			{
				u.first->gather(u.second);
				continue;
			}
		}
	}

	// All unit iterator
	for (auto u : Broodwar->self()->getUnits())
	{
		// Check if we are outside our base
		if (!outsideBase)
		{
			if (u->getType() != UnitTypes::Protoss_Probe && u->getType() != UnitTypes::Protoss_Zealot && getRegion(u->getTilePosition()) != getRegion(playerStartingTilePosition))
			{
				outsideBase = true;
			}
		}

		// Ignore the unit if it no longer exists, is locked down, maelstrommed, stassised, loaded, not powered, stuck or not completed
		if (!u || !u->exists() || u->isLockedDown() || u->isMaelstrommed() || u->isStasised() || u->isLoaded() || !u->isPowered() || !u->isCompleted() || u->isStuck())
		{
			continue;
		}
		else
		{
			storeAllyUnit(u, allyUnits);
		}

		// Remove loaded unit information
		if (u->isLoaded())
		{
			if (localEnemy.find(u) != localEnemy.end() && localAlly.find(u) != localAlly.end() && unitRadiusCheck.find(u) != unitRadiusCheck.end())
			{
				localEnemy.erase(u);
				localAlly.erase(u);
				unitRadiusCheck.erase(u);
			}
		}


#pragma region Probe Manager

		if (u->getType() == UnitTypes::Protoss_Probe)
		{
			if (u->getUnitsInRadius(64, Filter::IsEnemy).size() > 0 && allyStrength < enemyStrength && (u->getHitPoints() + u->getShields()) > 20)
			{
				assignCombat(u);
			}
			else if (find(combatProbe.begin(), combatProbe.end(), u) != combatProbe.end() && (u->getUnitsInRadius(64, Filter::IsEnemy).size() == 0 || allyStrength > enemyStrength || (u->getHitPoints() + u->getShields()) <= 20))
			{
				unAssignCombat(u);
				u->stop();
			}
			if (combatProbe.size() > 2 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) < 1)
			{
				// SCV rush probable
				enemyAggresion = true;
			}
			if (find(combatProbe.begin(), combatProbe.end(), u) != combatProbe.end())
			{
				u->attack(u->getClosestUnit(Filter::IsEnemy));
			}
			if (mineralProbeMap.find(u) == mineralProbeMap.end() && gasProbeMap.find(u) == gasProbeMap.end())
			{
				// Assign the probe a task (mineral, gas)
				assignProbe(u);
				continue;
			}

			// Crappy scouting method
			if (!scout)
			{
				scout = u;
			}
			if (u == scout)
			{
				if (Broodwar->self()->supplyUsed() >= 18 && scouting)
				{
					if (enemyBasePositions.size() > 0 && u->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsWorker && Filter::CanAttack).size() < 1)
					{
						Position one = Position(enemyBasePositions.at(0).x - 192, enemyBasePositions.at(0).y - 192);
						Position two = Position(enemyBasePositions.at(0).x + 192, enemyBasePositions.at(0).y - 192);
						Position three = Position(enemyBasePositions.at(0).x + 192, enemyBasePositions.at(0).y + 192);
						Position four = Position(enemyBasePositions.at(0).x - 192, enemyBasePositions.at(0).y + 192);
						if (u->getDistance(one) < 64)
						{
							u->move(two);
						}
						else if (u->getDistance(two) < 64)
						{
							u->move(three);
						}
						else if (u->getDistance(three) < 64)
						{
							u->move(four);
						}
						else if (u->getDistance(four) < 64)
						{
							u->move(one);
						}
						else if (u->getLastCommandFrame() + 100 < Broodwar->getFrameCount())
						{
							u->move(one);
						}
					}
					else if (enemyBasePositions.size() < 1)
					{
						if (u->isCarryingMinerals())
						{
							if (u->getLastCommand().getType() != UnitCommandTypes::Return_Cargo)
							{
								u->returnCargo();
							}
							break;
						}
						for (auto start : getStartLocations())
						{
							if (Broodwar->isExplored(start->getTilePosition()) == false)
							{
								u->move(start->getPosition());
								break;
							}
						}
					}
					else if (u->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsWorker && Filter::CanAttack).size() > 0)
					{
						u->stop();
						scouting = false;
					}
				}
			}
		}

#pragma endregion
#pragma region Building Manager
			{
				// If it's a Nexus and we need probes, check if we need probes, then train if needed
				if (u->getType().isResourceDepot())
				{
					for (auto m : mineralMap)
					{
						if (m.second < 2)
						{
							if (u->isIdle() && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) < 60 && (Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + queuedMineral + reservedMineral))
							{
								u->train(UnitTypes::Protoss_Probe);
								break;
							}
						}
					}
					for (auto g : gasMap)
					{
						if (g.second < 3)
						{
							if (u->isIdle() && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) < 60 && (Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + queuedMineral + reservedMineral))
							{
								u->train(UnitTypes::Protoss_Probe);
								break;
							}
						}
					}
					if (u->isCompleted())
					{
						// If there are no pylons around it, build one so we can make cannons
						if (u->getUnitsInRadius(128, Filter::GetType == UnitTypes::Enum::Protoss_Pylon).size() == 0 && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Nexus) > 1)
						{
							TilePosition here = getBuildLocationNear(UnitTypes::Protoss_Pylon, u->getTilePosition());
							Unit builder = Broodwar->getClosestUnit(u->getPosition(), Filter::IsAlly && Filter::IsWorker);
							if (builder && builder->exists())
							{
								builder->build(UnitTypes::Protoss_Pylon, here);
							}
						}

						updateDefenses(u, myNexus);

						if (Broodwar->self()->completedUnitCount(Protoss_Forge) > 0 && myNexus[u].getStaticD() < 2)
						{
							
							TilePosition here = cannonManager(u->getTilePosition());
							Unit builder = Broodwar->getClosestUnit(Position(here), Filter::IsAlly && Filter::IsWorker && !Filter::IsCarryingSomething && !Filter::IsGatheringGas);
							// If the Tile Position and Builder are valid
							if (here != TilePositions::None && builder)
							{
								// Queue at this building type a pair of building placement and builder
								queuedBuildings.emplace(Protoss_Photon_Cannon, make_pair(here, builder));
							}
						}
					}
				}
				else if (u->getType() == UnitTypes::Protoss_Assimilator && gasMap.find(u) == gasMap.end())
				{
					gasMap.emplace(u, 0);
				}

				// If it's a building capable of production, send to production manager
				else if (u->getType().isBuilding() && u->getType() != UnitTypes::Protoss_Pylon && u->getType() != UnitTypes::Protoss_Nexus)
				{
					productionManager(u);
					continue;
				}
			}
#pragma endregion

#pragma region Army Unit Manager	
			{
				if (u->getType() == UnitTypes::Protoss_Dragoon || u->getType() == UnitTypes::Protoss_Zealot || u->getType() == UnitTypes::Protoss_Dark_Templar || u->getType() == UnitTypes::Protoss_Archon)
				{
					unitGetCommand(u);
					continue;
				}
				else if (u->getType() == UnitTypes::Protoss_Shuttle)
				{
					shuttleManager(u);
					/*if (harassShuttleID.size() < 1 || find(harassShuttleID.begin(), harassShuttleID.end(), u->getID()) != harassShuttleID.end())
					{
					shuttleHarass(u);
					harassShuttleID.push_back(u->getID());
					}
					else
					{
					shuttleManager(u);
					}*/
					continue;
				}
				else if (u->getType() == UnitTypes::Protoss_Observer)
				{
					observerManager(u);
					continue;
				}
				else if (u->getType() == UnitTypes::Protoss_Reaver)
				{
					reaverManager(u);
					/*if (harassReaverID.size() < 1 || find(harassReaverID.begin(), harassReaverID.end(), u->getID()) != harassReaverID.end())
					{
					harassReaverID.push_back(u->getID());
					}*/
					continue;
				}
				else if (u->getType() == UnitTypes::Protoss_High_Templar)
				{
					templarManager(u);
					continue;
				}
				else if (u->getType() == UnitTypes::Protoss_Carrier)
				{
					carrierManager(u);
					continue;
				}
				else if (u->getType() == UnitTypes::Protoss_Arbiter)
				{
					arbiterManager(u);
					continue;
				}
				else if (u->getType() == UnitTypes::Protoss_Corsair)
				{
					corsairManager(u);
					continue;
				}
			}

#pragma endregion
	}
#pragma endregion

#pragma region Resource Iterator
	// Constant update of mineral fields
	for (auto r : Broodwar->neutral()->getUnits())
	{
		if (r && r->exists())
		{
			if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) > 0)
			{
				if (r->getType().isMineralField() && r->getDistance(r->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Nexus)->getPosition()) < 320)
				{
					// If the mineral field isn't found in the map, it creates it with 0 workers currently assigned to it
					if (mineralMap.find(r) == mineralMap.end())
					{
						mineralMap.emplace(r, 0);
					}
				}
				if (r->getType() == UnitTypes::Resource_Vespene_Geyser && r->getDistance(r->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Nexus)->getPosition()) < 320)
				{
					// If the vespene geyser isn't found in the map, it creates it with 0 workers currently assigned to it
					if (find(geysers.begin(), geysers.end(), r) == geysers.end())
					{
						geysers.push_back(r);
					}
				}
			}
		}
	}
#pragma endregion
}

void McRave::onSendText(std::string text)
{
	// Disable/enable HUD
	if (text == "/master")
	{
		if (masterDraw)
		{
			masterDraw = false;
		}
		else
		{
			masterDraw = true;
		}
	}

	if (text == "/calc")
	{
		if (calculationDraw)
		{
			calculationDraw = false;
		}
		else
		{
			calculationDraw = true;
		}
	}
	// Else send the text to the game if it is not being processed
	Broodwar->sendText("%s", text.c_str());
}

void McRave::onReceiveText(BWAPI::Player player, std::string text)
{
}

void McRave::onPlayerLeft(BWAPI::Player player)
{
	Broodwar->sendText("GG %s!", player->getName().c_str());
}

void McRave::onNukeDetect(BWAPI::Position target)
{

	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void McRave::onUnitDiscover(BWAPI::Unit unit)
{
	if (unit->getType().isMineralField() && unit->getResources() == 0 && find(boulders.begin(), boulders.end(), unit) == boulders.end() && unit->getDistance(playerStartingPosition) < 1280)
	{
		boulders.push_back(unit);
	}
	// If unit not owned by player
	if (unit->getPlayer() == Broodwar->enemy())
	{
		if (unit->getType().isBuilding())
		{
			if (enemyBasePositions.size() == 0)
			{
				enemyStartingTilePosition = getNearestBaseLocation(unit->getPosition())->getTilePosition();
				enemyStartingPosition = getNearestBaseLocation(unit->getPosition())->getPosition();
				if (!Broodwar->isVisible(enemyStartingTilePosition))
				{
					enemyBasePositions.push_back(enemyStartingPosition);
				}
				path = theMap.GetPath(playerStartingPosition, enemyStartingPosition);

				// For each chokepoint, set a 10 tile radius of "no fly zone"
				for (auto position : path)
				{
					// Convert walk position (8x8) to tile position (32x32) = divide by 4
					int x = (position->Center().x / 4);
					int y = (position->Center().y / 4);

					int x1 = (position->Center().x / 4) - 15;
					int y1 = (position->Center().y / 4) - 15;

					for (int i = x1; i <= x1 + 30; i++)
					{
						for (int j = y1; j <= y1 + 30; j++)
						{
							if ((x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight()) && TilePosition(i, j).getDistance(TilePosition(x, y)) < 320)
							{
								shuttleHeatmap[i][j] = 256;
							}
						}
					}
				}
			}
		}
		if (unit->getType().isResourceDepot() && find(enemyBasePositions.begin(), enemyBasePositions.end(), getNearestBaseLocation(unit->getPosition())->getPosition()) == enemyBasePositions.end())
		{
			enemyBasePositions.push_back(getNearestBaseLocation(unit->getPosition())->getPosition());
		}
	}
}

void McRave::onUnitEvade(BWAPI::Unit unit)
{
}

void McRave::onUnitShow(BWAPI::Unit unit)
{
}

void McRave::onUnitHide(BWAPI::Unit unit)
{
}

void McRave::onUnitCreate(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self() && Broodwar->getFrameCount() >= 100)
	{
		queuedBuildings.erase(unit->getType());
		if (unit->getType() == UnitTypes::Enum::Protoss_Nexus)
		{
			allyTerritory.push_back(getRegion(unit->getPosition()));
			forceExpand = 0;
		}
	}	
}

void McRave::onUnitDestroy(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{
		allyUnits.erase(unit->getID());
		localEnemy.erase(unit);
		localAlly.erase(unit);
		unitRadiusCheck.erase(unit);
		unitsCurrentTarget.erase(unit);
		// For probes, adjust the resource maps to align properly
		if (unit->getType() == UnitTypes::Protoss_Probe)
		{
			if (gasProbeMap.find(unit) != gasProbeMap.end())
			{
				gasMap[(gasProbeMap.at(unit))] -= 1;
				gasProbeMap.erase(unit);
			}
			if (mineralProbeMap.find(unit) != mineralProbeMap.end())
			{
				mineralMap[(mineralProbeMap.at(unit).first)] -= 1;
				mineralProbeMap.erase(unit);
			}
			if (find(combatProbe.begin(), combatProbe.end(), unit) != combatProbe.end())
			{
				combatProbe.erase(find(combatProbe.begin(), combatProbe.end(), unit));
			}
		}
		else if (unit->getType() == UnitTypes::Protoss_Assimilator)
		{
			gasMap.erase(unit);
		}
		else if (unit->getType() == UnitTypes::Protoss_Nexus)
		{			
			if (find(allyTerritory.begin(), allyTerritory.end(), getRegion(unit->getTilePosition())) != allyTerritory.end())
			{
				allyTerritory.erase(find(allyTerritory.begin(), allyTerritory.end(), getRegion(unit->getTilePosition())));
			}
		}
	}
	else if (unit->getPlayer() == Broodwar->enemy())
	{
		enemyUnits.erase(unit->getID());
		if (unit->getType().isResourceDepot() && find(enemyBasePositions.begin(), enemyBasePositions.end(), unit->getPosition()) != enemyBasePositions.end())
		{
			enemyBasePositions.erase(find(enemyBasePositions.begin(), enemyBasePositions.end(), unit->getPosition()));
		}
	}
	if (unit->getType().isMineralField() && unit->getInitialResources() > 0)
	{
		TilePosition closestNexus = Broodwar->getClosestUnit(unit->getPosition(), Filter::IsResourceDepot)->getTilePosition();
		if (find(activeExpansion.begin(), activeExpansion.end(), closestNexus) != activeExpansion.end())
		{
			activeExpansion.erase(find(activeExpansion.begin(), activeExpansion.end(), closestNexus));
			inactiveNexusCnt++;
		}
		mineralMap.erase(unit);
		for (auto m : mineralProbeMap)
		{
			if (m.second.first == unit)
			{
				mineralProbeMap.erase(m.first);
			}
		}

	}
}

void McRave::onUnitMorph(BWAPI::Unit unit)
{

}

void McRave::onUnitRenegade(BWAPI::Unit unit)
{
}

void McRave::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void McRave::onUnitComplete(BWAPI::Unit unit)
{

}

DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();

	analyzed = true;
	analysis_just_finished = true;
	return 0;
}

void McRave::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for (const auto& baseLocation : BWTA::getBaseLocations()) {
		TilePosition p = baseLocation->getTilePosition();

		//draw outline of center location
		Position leftTop(p.x * TILE_SIZE, p.y * TILE_SIZE);
		Position rightBottom(leftTop.x + 4 * TILE_SIZE, leftTop.y + 3 * TILE_SIZE);
		Broodwar->drawBoxMap(leftTop, rightBottom, Colors::Blue);

		//draw a circle at each mineral patch
		for (const auto& mineral : baseLocation->getStaticMinerals()) {
			Broodwar->drawCircleMap(mineral->getInitialPosition(), 30, Colors::Cyan);
		}

		//draw the outlines of Vespene geysers
		for (const auto& geyser : baseLocation->getGeysers()) {
			TilePosition p1 = geyser->getInitialTilePosition();
			Position leftTop1(p1.x * TILE_SIZE, p1.y * TILE_SIZE);
			Position rightBottom1(leftTop1.x + 4 * TILE_SIZE, leftTop1.y + 2 * TILE_SIZE);
			Broodwar->drawBoxMap(leftTop1, rightBottom1, Colors::Orange);
		}

		//if this is an island expansion, draw a yellow circle around the base location
		if (baseLocation->isIsland()) {
			Broodwar->drawCircleMap(baseLocation->getPosition(), 80, Colors::Yellow);
		}
	}

	//we will iterate through all the regions and ...
	for (const auto& region : BWTA::getRegions()) {
		// draw the polygon outline of it in green
		BWTA::Polygon p = region->getPolygon();
		for (size_t j = 0; j < p.size(); ++j) {
			Position point1 = p[j];
			Position point2 = p[(j + 1) % p.size()];
			Broodwar->drawLineMap(point1, point2, Colors::Green);
		}
		// visualize the chokepoints with red lines
		for (auto const& chokepoint : region->getChokepoints()) {
			Position point1 = chokepoint->getSides().first;
			Position point2 = chokepoint->getSides().second;
			Broodwar->drawLineMap(point1, point2, Colors::Red);
		}
	}
}