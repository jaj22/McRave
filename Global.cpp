// McRave is made by Christian McCrave
// Twitch nicknamed it McRave \o/
// For any questions, email christianmccrave@gmail.com
// Bot started 01/03/2017

// Includes
#include "Header.h"
#include "Global.h"

// Author notes:
// Classes
// UnitScore class
// Idle Buildings class
// Building Tracker class (desired buildings)

// Test static defenses
// Crash testing when losing
// Zerg don't move out early
// Boulder removal, heartbreak ridge is an issue
// If scout dies, no base found

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
bool calculationDraw = true;

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
#pragma region Territory Manager
	if (!analyzeHome)
	{
		if (territory.size() < 1)
		{
			allyTerritory.emplace(getRegion(playerStartingPosition));
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
						if (Chokepoint->getWidth() > 200)
						{
							allyTerritory.emplace(Chokepoint->getRegions().first);
							allyTerritory.emplace(Chokepoint->getRegions().second);
						}
					}
					// Check if every chokepoint is connected to an ally territory, in which case add this to our territory (connecting regions)
					if (allyTerritory.find(Chokepoint->getRegions().first) != allyTerritory.end() || allyTerritory.find(Chokepoint->getRegions().second) != allyTerritory.end())
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
				if (merge == true && allyTerritory.find(region) == allyTerritory.end())
				{
					allyTerritory.emplace(region);
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
					if (allyTerritory.find(Chokepoint->getRegions().first) != allyTerritory.end() && allyTerritory.find(Chokepoint->getRegions().second) != allyTerritory.end())
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
			break;
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
					int distances = getGroundDistance2(base->getTilePosition(), playerStartingTilePosition) - getGroundDistance2(base->getTilePosition(), enemyStartingTilePosition) - base->getTilePosition().getDistance(TilePosition(Broodwar->mapWidth(), Broodwar->mapHeight()));
					if (base->isMineralOnly() && base->getPosition().getDistance(playerStartingPosition) < 2560)
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
		for (auto pos : enemyBasePositions)
		{
			if (Broodwar->isVisible(TilePosition(pos)) && Broodwar->getUnitsInRadius(pos, 128, Filter::IsEnemy).size() == 0)
			{
				enemyBasePositions.erase(find(enemyBasePositions.begin(), enemyBasePositions.end(), pos));
				break;
			}
		}
	}
#pragma endregion
#pragma region Grids
	double strongest = 0.0;
	// For each tile, draw the current threat onto the tile
	for (int x = 0; x <= Broodwar->mapWidth(); x++)
	{
		for (int y = 0; y <= Broodwar->mapHeight(); y++)
		{
			if (enemyGroundStrengthGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%.2f", enemyGroundStrengthGrid[x][y]);
			}
			if (shuttleHeatmap[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", shuttleHeatmap[x][y]);
			}
			if (allyClusterGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%.2f", allyClusterGrid[x][y]);
			}
			if (enemyGroundClusterGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", enemyGroundClusterGrid[x][y]);
			}
			if (enemyAirClusterGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", enemyAirClusterGrid[x][y]);
			}
			if (resourceGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", resourceGrid[x][y]);
			}
			if (allyDetectorGrid[x][y] > 0)
			{
				//Broodwar->drawTextMap(x * 32, y * 32, "%d", allyDetectorGrid[x][y]);
			}

			if (allyClusterGrid[x][y] > strongest)
			{
				supportPosition = Position(x * 32, y * 32);
				strongest = allyClusterGrid[x][y];
			}

			// Reset strength grids
			enemyGroundStrengthGrid[x][y] = 0;
			enemyAirStrengthGrid[x][y] = 0;

			// Reset cluster grids
			enemyGroundClusterGrid[x][y] = 0;
			enemyAirClusterGrid[x][y] = 0;
			allyClusterGrid[x][y] = 0;

			// Reset other grids
			allyDetectorGrid[x][y] = 0;
		}
	}
#pragma region Ally Grids
	for (auto &u : allyUnits)
	{
		TilePosition unitTilePosition = TilePosition(u.second.getPosition());
		int offsetX = u.second.getPosition().x % 32;
		int offsetY = u.second.getPosition().y % 32;

		// Ally cluster
		if (!u.second.getUnitType().isWorker() && !u.second.getUnitType().isBuilding())
		{
			for (int x = unitTilePosition.x - 5; x <= unitTilePosition.x + 6; x++)
			{
				for (int y = unitTilePosition.y - 5; y <= unitTilePosition.y + 6; y++)
				{
					if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && (u.second.getPosition() + Position(offsetX, offsetY)).getDistance(Position((x * 32 + offsetX), (y * 32 + offsetY))) <= 160)
					{
						allyClusterGrid[x][y] += 1;
					}
				}
			}
		}

		// Ally detection
		if (u.second.getUnitType() == UnitTypes::Protoss_Observer)
		{
			for (int x = unitTilePosition.x - 5; x <= unitTilePosition.x + 6; x++)
			{
				for (int y = unitTilePosition.y - 5; y <= unitTilePosition.y + 6; y++)
				{
					if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && (u.second.getPosition() + Position(offsetX, offsetY)).getDistance(Position((x * 32 + offsetX), (y * 32 + offsetY))) <= 160)
					{
						allyDetectorGrid[x][y] += 1;
					}
				}
			}
		}
	}
#pragma endregion
#pragma region Enemy Grids
	for (auto &u : enemyUnits)
	{

		// For each enemy unit, add its attack value to each tile it is in range of
		TilePosition unitTilePosition = TilePosition(u.second.getPosition());
		if (unitGetStrength(u.second.getUnitType()) > 1.0 && u.second.getDeadFrame() == 0)
		{
			// Store range in class
			int offsetX = u.second.getPosition().x % 32;
			int offsetY = u.second.getPosition().y % 32;
			int range = (int)u.second.getRange();
			// Making sure we properly analyze the threat of melee units without adding range to ranged units
			if (range < 32)
			{
				range = (range + 64) / 32;
			}
			else
			{
				range = range / 32;
			}
			// The + 1 is because we need to still check an additional tile
			for (int x = unitTilePosition.x - range; x <= unitTilePosition.x + range + 1; x++)
			{
				for (int y = unitTilePosition.y - range; y <= unitTilePosition.y + range + 1; y++)
				{
					if ((u.second.getPosition() + Position(offsetX, offsetY)).getDistance(Position((x * 32 + offsetX), (y * 32 + offsetY))) <= (range * 32) && x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight())
					{
						enemyGroundStrengthGrid[x][y] += u.second.getStrength();
					}
				}
			}
		}
		if (u.second.getUnitType().airWeapon().damageAmount() > 0)
		{
			int range = (u.second.getUnitType().airWeapon().maxRange()) / 32;
			// The + 1 is because we need to still check an additional tile
			for (int x = unitTilePosition.x - range; x <= unitTilePosition.x + range + 1; x++)
			{
				for (int y = unitTilePosition.y - range; y <= unitTilePosition.y + range + 1; y++)
				{
					if (u.second.getPosition().getDistance(Position((x * 32), (y * 32))) <= (range * 32) && x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight())
					{
						enemyAirStrengthGrid[x][y] += u.second.getStrength();
					}
				}
			}
		}
		if (u.first && u.first->exists() && !u.second.getUnitType().isBuilding() && !u.first->isStasised() && !u.first->isMaelstrommed())
		{
			// Cluster heatmap for psi/stasis (96x96)			
			for (int x = unitTilePosition.x - 1; x <= unitTilePosition.x + 1; x++)
			{
				for (int y = unitTilePosition.y - 1; y <= unitTilePosition.y + 1; y++)
				{
					if ((x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight()))
					{
						if (!u.second.getUnitType().isFlyer())
						{
							enemyGroundClusterGrid[x][y] += 1;
						}
						else
						{
							enemyAirClusterGrid[x][y] += 1;
						}
						if (u.second.getUnitType() == UnitTypes::Terran_Siege_Tank_Tank_Mode || u.second.getUnitType() == UnitTypes::Terran_Siege_Tank_Siege_Mode)
						{
							tankClusterHeatmap[x][y] += 1;
						}
					}
				}
			}
		}
	}
#pragma endregion

#pragma endregion

#pragma region HUD
	if (masterDraw)
	{
		int offset = 0;

		// Show what buildings we want
		for (auto b : buildingDesired)
		{
			if (b.second > 0)
			{
				Broodwar->drawTextScreen(0, offset, "%s : %d", b.first.toString().c_str(), b.second);
				offset = offset + 10;
			}
		}

		// Saturated
		Broodwar->drawTextScreen(200, 40, "%d", saturated);

		// Display some information about our queued resources required for structure building			
		Broodwar->drawTextScreen(200, 0, "Current Strategy: %s", currentStrategy.c_str());

		// Display frame count
		Broodwar->drawTextScreen(200, 10, "%d", Broodwar->getFrameCount());

		// Display global strength calculations	
		Broodwar->drawTextScreen(500, 20, "A: %.2f    E: %.2f", allyStrength, enemyStrength);

		// Display remaining minerals on each mineral patch that is near our Nexus
		for (auto r : myMinerals)
		{
			Broodwar->drawTextMap(r.second.getPosition() + Position(-8, 8), "%c%d", Text::White, r.second.getRemainingResources());
		}

		// Display remaining gas on each geyser that is near our Nexus
		for (auto r : myGas)
		{
			Broodwar->drawTextMap(r.second.getPosition() + Position(-8, 32), "%c%d", Text::Green, r.second.getRemainingResources());
		}

		// Display static defense count
		for (auto n : myNexus)
		{
			Broodwar->drawTextMap(n.first->getPosition(), "Static Defenses: %d", n.second.getStaticD());
		}

		// Display Goon/Zealot scoring
		offset = 0;
		for (auto t : unitScore)
		{
			Broodwar->drawTextScreen(500, 200 + offset, "%s : %.2f", t.first.toString().c_str(), t.second);
			offset = offset + 10;
		}

		// Show building placements
		for (auto b : queuedBuildings)
		{
			Broodwar->drawBoxMap(Position(b.second.first), Position((b.second.first.x + b.first.tileWidth()) * 32, (b.second.first.y + b.first.tileHeight()) * 32), playerColor);
		}

		// Show static defense positions
		for (auto nexus : myNexus)
		{
			//Broodwar->drawCircleMap(Position(nexus.second.getStaticP()), 8, Colors::Red, true);
		}

		// Show support position
		//Broodwar->drawCircleMap(supportPosition, 8, playerColor, true);

		// Show expansions
		if (analyzed)
		{
			for (int i = 0; i <= (int)activeExpansion.size() - 1; i++)
			{
				Broodwar->drawTextMap(48 + activeExpansion.at(i).x * 32, 104 + activeExpansion.at(i).y * 32, "%cBase %d", Broodwar->self()->getTextColor(), i, Colors::White);
			}

		}
	}

#pragma endregion

#pragma region EnemyUnits

	// Reset variables for storing sizes and strength
	eSmall = 0, eMedium = 0, eLarge = 0;
	enemyStrength = 0.0;

	for (auto u : Broodwar->enemy()->getUnits())
	{
		// Update unit
		if (u && u->exists() && u->isCompleted())
		{
			storeEnemyUnit(u, enemyUnits);
		}
	}

	// Reset unit score
	for (auto &unit : unitScore)
	{
		unit.second = 0;
	}

	// If unit has been dead for over 500 frames, erase it (needed manual loop)
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

#pragma region EnemyInfo
	// Stored enemy units iterator
	for (auto &u : enemyUnits)
	{
		// If nullptr, continue
		if (!u.first)
		{
			continue;
		}
		// If deadframe is 0, unit is alive still
		if (u.second.getDeadFrame() == 0)
		{
			// If tile is visible but unit is not, remove position
			if (!u.first->exists() && u.second.getPosition() != Positions::None && Broodwar->isVisible(TilePosition(u.second.getPosition())))
			{
				u.second.setPosition(Positions::None);
			}

			// Strength based calculations ignore workers and buildings
			if ((u.second.getUnitType().isBuilding() && u.second.getStrength() > 1.0) || (!u.second.getUnitType().isBuilding() && !u.second.getUnitType().isWorker()) || u.first->exists() && allyTerritory.find(getRegion(u.first->getTilePosition())) != allyTerritory.end())
			{
				// Add composition and strength
				enemyComposition[u.second.getUnitType()] += 1;
				enemyStrength += u.second.getStrength();
			}

			// Drawing
			if (masterDraw)
			{
				if (u.second.getUnitType().isBuilding())
				{
					Broodwar->drawEllipseMap(u.second.getPosition(), u.second.getUnitType().height() / 2, u.second.getUnitType().height() / 3, Broodwar->enemy()->getColor());
				}
				else
				{
					Broodwar->drawEllipseMap(u.second.getPosition() + Position(0, u.second.getUnitType().height() / 2), u.second.getUnitType().height() / 2, u.second.getUnitType().height() / 3, Broodwar->enemy()->getColor());
				}
				if (calculationDraw)
				{
					Broodwar->drawTextMap(u.second.getPosition() + Position(-8, -8), "%c%.2f", Broodwar->enemy()->getTextColor(), u.second.getStrength());
				}

			}

			// Store size of unit
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

		// If unit is dead
		else if (u.second.getDeadFrame() != 0)
		{
			// Add a portion of the strength to ally strength
			allyStrength += unitGetStrength(u.second.getUnitType()) * 1 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
			if (calculationDraw)
			{
				Broodwar->drawTextMap(u.second.getPosition() + Position(-8, 8), "%c%d", Broodwar->enemy()->getTextColor(), Broodwar->getFrameCount() - u.second.getDeadFrame());
			}
		}
	}
#pragma endregion

#pragma region Composition
	int offset = 0;
	for (auto &t : enemyComposition)
	{
		// For each type, add a score to production based on the unit count divided by our current unit count
		unitScoreUpdate(t.first, t.second);
		if (t.first != UnitTypes::None && t.second > 0.0)
		{
			Broodwar->drawTextScreen(500, 50 + offset, "%s : %d", t.first.toString().c_str(), t.second);
			offset = offset + 10;
		}

		if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) < 2)
		{
			if (t.first == UnitTypes::Terran_Bunker || (t.first == UnitTypes::Zerg_Sunken_Colony && t.second >= 2) || (t.first == UnitTypes::Protoss_Photon_Cannon && t.second >= 2))
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
#pragma endregion

#pragma endregion

#pragma region AllyUnits
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

		// Ignore the unit if it no longer exists, is locked down, maelstrommed, stassised, not powered or not completed
		if (!u || !u->exists() || u->isLockedDown() || u->isMaelstrommed() || u->isStasised() || !u->isPowered() || !u->isCompleted())
		{
			continue;
		}
		else if (u->getType() == UnitTypes::Protoss_Scarab)
		{
			continue;
		}


		// Return if not latency frame
		if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 && allyUnits.find(u) != allyUnits.end())
		{
			allyUnits[u].setPosition(u->getPosition());
			continue;
		}
		// Update unit
		else
		{
			storeAllyUnit(u, allyUnits);
		}

		// Skip loaded/stuck units otherwise
		if (u->isLoaded() || u->isStuck())
		{
			continue;
		}
#pragma region Probe Manager
		if (u->getType() == UnitTypes::Protoss_Probe)
		{
			// Store probes
			if (myProbes.find(u) == myProbes.end())
			{
				storeProbe(u, myProbes);
			}

			// Crappy scouting method
			if (!scout)
			{
				scout = u;
			}
			if (u == scout)
			{
				if (supply >= 18 && scouting)
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
			if (u->getType().isResourceDepot() && u->isIdle() && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) < 60 && (Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + queuedMineral + reservedMineral))
			{
				if (!saturated)
				{
					u->train(UnitTypes::Protoss_Probe);
				}

				// Draw the tile
				//TilePosition here = cannonManager(myNexus[u].getStaticP(), UnitTypes::Protoss_Pylon);
				//Broodwar->drawCircleMap(Position(here), 16, Colors::Red);
				updateDefenses(u, myNexus);

				// Get tile position of pylon placement for each expansion if needed, then cannon placement (using cannon managers function)
				if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) > 2)
				{

					TilePosition here;
					Unit builder = Broodwar->getClosestUnit(Position(here), Filter::IsAlly && Filter::IsWorker && !Filter::IsCarryingSomething && !Filter::IsGatheringGas);

					if (Broodwar->canBuildHere(myNexus[u].getStaticP(), UnitTypes::Protoss_Photon_Cannon) && Broodwar->hasPower(myNexus[u].getStaticP(), UnitTypes::Protoss_Photon_Cannon))
					{
						here = cannonManager(myNexus[u].getStaticP(), UnitTypes::Protoss_Photon_Cannon);
						if (here != TilePositions::None && builder)
						{
							queuedBuildings.emplace(Protoss_Photon_Cannon, make_pair(here, builder));
						}
					}
					else
					{
						here = cannonManager(myNexus[u].getStaticP(), UnitTypes::Protoss_Pylon);
						if (here != TilePositions::None && builder)
						{
							queuedBuildings.emplace(Protoss_Pylon, make_pair(here, builder));
						}
					}
				}
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

#pragma region AllyInfo	

	// Reset ally info variables
	aSmall = 0, aMedium = 0, aLarge = 0;
	allyStrength = 0.0;
	outsideBase = false;

	// If unit has been dead for over 500 frames, erase it (needed manual loop)
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

	// Check through all alive units or units dead within 500 frames
	for (auto &u : allyUnits)
	{
		// If deadframe is 0, unit is alive still
		if (u.second.getDeadFrame() == 0)
		{
			// Strength based calculations ignore workers and buildings
			if (!u.second.getUnitType().isWorker() && !u.second.getUnitType().isBuilding())
			{
				// Add strength				
				allyStrength += u.second.getStrength();

				// Set last command frame
				if (u.first->isStartingAttack())
				{
					u.second.setLastCommandFrame(Broodwar->getFrameCount());
				}

				// Drawing
				if (masterDraw)
				{
					if (u.second.getTargetPosition() != Positions::None && u.second.getPosition() != Positions::None && u.first->getDistance(u.second.getTargetPosition()) < 500)
					{
						Broodwar->drawLineMap(u.second.getPosition(), u.second.getTargetPosition(), playerColor);
					}
					if (calculationDraw)
					{
						if (u.second.getLocal() < 0)
						{
							Broodwar->drawTextMap(u.second.getPosition() + Position(-8, 8), "%c%.2f", Broodwar->enemy()->getTextColor(), (u.second.getLocal())); //, "\n%s", u.second.getCommand().c_str(), Text::Enum::Default);
						}
						else if (u.second.getLocal() > 0)
						{
							Broodwar->drawTextMap(u.second.getPosition() + Position(-8, 8), "%c%.2f", Broodwar->self()->getTextColor(), u.second.getLocal()); //, "\n%s", u.second.getCommand().c_str(), Text::Enum::Default);
						}
						else
						{
							Broodwar->drawTextMap(u.second.getPosition() + Position(-8, 8), "%c%.2f", Text::Default, u.second.getLocal()); //, "\n%s", u.second.getCommand().c_str(), Text::Enum::Default);
						}
					}
				}

				//Broodwar->drawEllipseMap(u.second.getPosition(), u.second.getUnitType().height() / 2, u.second.getUnitType().height() / 3, Broodwar->self()->getColor());

				// Store size of unit
				if (u.second.getUnitType().size() == UnitSizeTypes::Small)
				{
					aSmall++;
				}
				else if (u.second.getUnitType().size() == UnitSizeTypes::Medium)
				{
					aMedium++;
				}
				else
				{
					aLarge++;
				}
			}
		}
		else
		{
			enemyStrength += unitGetStrength(u.second.getUnitType()) * 0.5 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
			if (calculationDraw)
			{
				Broodwar->drawTextMap(u.second.getPosition() + Position(-8, 8), "%c%d", Broodwar->self()->getTextColor(), Broodwar->getFrameCount() - u.second.getDeadFrame());
			}
		}
	}

#pragma endregion

#pragma endregion

#pragma region Neutrals
	for (auto &r : Broodwar->neutral()->getUnits())
	{
		if (r && r->exists())
		{
			if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) > 0 && (allyTerritory.find(getRegion(r->getTilePosition())) != allyTerritory.end() || (Broodwar->getFrameCount() > 5 && Broodwar->getFrameCount() < 50)))
			{
				if (r->getType().isMineralField() && myMinerals.find(r) == myMinerals.end() && r->getInitialResources() > 0)
				{
					storeMineral(r, myMinerals);
				}

				if (myGas.find(r) == myGas.end() && r->getType() == UnitTypes::Resource_Vespene_Geyser)
				{
					storeGas(r, myGas);
					geysers.push_back(r);
				}
			}
		}
	}

	// Assume saturated so check happens
	saturated = true;
	for (auto &m : myMinerals)
	{
		if (m.first->exists())
		{
			m.second.setRemainingResources(m.first->getResources());
			// Update resource grid
			for (int x = m.second.getTilePosition().x - 2; x <= m.second.getTilePosition().x + m.second.getUnitType().tileWidth() + 2; x++)
			{
				for (int y = m.second.getTilePosition().y - 2; y <= m.second.getTilePosition().y + m.second.getUnitType().tileHeight() + 2; y++)
				{
					if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && m.second.getPosition().getDistance(m.second.getClosestNexus()->getPosition()) > Position(x * 32, y * 32).getDistance(m.second.getClosestNexus()->getPosition()))
					{
						resourceGrid[x][y] = 1;
					}
				}
			}
		}
		if (saturated && m.second.getGathererCount() < 2)
		{
			saturated = false;
		}
	}

	for (auto &g : myGas)
	{
		if (g.first->exists())
		{
			g.second.setUnitType(g.first->getType());
			g.second.setRemainingResources(g.first->getResources());

			// Update resource grid
			for (int x = g.second.getTilePosition().x - 1; x <= g.second.getTilePosition().x + g.second.getUnitType().tileWidth() + 1; x++)
			{
				for (int y = g.second.getTilePosition().y - 1; y <= g.second.getTilePosition().y + g.second.getUnitType().tileHeight() + 1; y++)
				{
					if (x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight() && g.second.getPosition().getDistance(g.second.getClosestNexus()->getPosition()) > Position(x * 32, y * 32).getDistance(g.second.getClosestNexus()->getPosition()))
					{
						resourceGrid[x][y] = 1;
					}
				}
			}
		}
		if (g.second.getGathererCount() < 3)
		{
			saturated = false;
			break;
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

	for (auto &b : queuedBuildings)
	{
		queuedMineral += b.first.mineralPrice();
		queuedGas += b.first.gasPrice();

		// If probe died, replace the probe
		if (!b.second.second->exists())
		{
			b.second.second = Broodwar->getClosestUnit(Position(b.second.first), Filter::IsAlly && Filter::IsWorker && !Filter::IsCarryingSomething && !Filter::IsGatheringGas);
			continue;
		}

		// If can't build here right now and the tile is visible, replace the building position
		if (!Broodwar->canBuildHere(b.second.first, b.first, b.second.second) && Broodwar->isVisible(b.second.first))
		{
			b.second.first = buildingManager(b.first);
			continue;
		}

		// If the Probe has a target
		if (b.second.second->getTarget())
		{
			// If the target has a resource count of 0 (mineral blocking a ramp), let Probe continue mining it
			if (b.second.second->getTarget()->getResources() == 0)
			{
				continue;
			}
		}

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

#pragma region Probe Commands
	// For each Probe mapped to gather minerals
	for (auto &u : myProbes)
	{
		// If no valid target, try to get a new one
		if (!u.second.getTarget())
		{
			u.second.setTarget(assignResource(myMinerals, myGas));
			// Any idle Probes can gather closest mineral field until they are assigned again
			if (u.first->isIdle() && u.first->getClosestUnit(Filter::IsMineralField))
			{
				u.first->gather(u.first->getClosestUnit(Filter::IsMineralField));
				continue;
			}
			continue;
		}

		// Attack units in mineral line
		if (resourceGrid[u.first->getTilePosition().x][u.first->getTilePosition().y] > 0 && u.first->getUnitsInRadius(64, Filter::IsEnemy).size() > 0 && (u.first->getHitPoints() + u.first->getShields()) > 20)
		{
			u.first->attack(u.first->getClosestUnit(Filter::IsEnemy, 320));
		}
		else if (u.first->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && (resourceGrid[u.first->getTilePosition().x][u.first->getTilePosition().y] == 0 || (u.first->getHitPoints() + u.first->getShields()) <= 20))
		{
			u.first->stop();
		}

		// Else command probe
		if (u.first && u.first->exists())
		{
			// Draw on every frame
			if (u.first && u.second.getTarget())
			{
				Broodwar->drawLineMap(u.first->getPosition(), u.second.getTarget()->getPosition(), playerColor);
			}

			// Return if not latency frame
			if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
			{
				continue;
			}

			// If idle and carrying minerals, return cargo			
			if (u.first->isIdle() && u.first->isCarryingMinerals())
			{
				u.first->returnCargo();
				continue;
			}

			// If not scouting and there's boulders to remove
			if (!scouting && boulders.size() > 0)
			{
				for (auto b : boulders)
				{
					if (!u.first->isGatheringMinerals() && u.first->getDistance(b) < 512)
					{
						u.first->gather(b);
						break;
					}
				}
			}

			// If we have been given a command this frame already, continue
			if (u.first->getLastCommandFrame() >= Broodwar->getFrameCount() && (u.first->getLastCommand().getType() == UnitCommandTypes::Move || u.first->getLastCommand().getType() == UnitCommandTypes::Build))
			{
				continue;
			}


			// If idle and not targeting the mineral field the Probe is mapped to
			if (u.first->isIdle() || (u.first->isGatheringMinerals() && !u.first->isCarryingMinerals() && u.first->getTarget() != u.second.getTarget()))
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
				if (u.second.getTarget() && u.second.getTarget()->exists())
				{
					u.first->gather(u.second.getTarget());
					continue;
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
	if (unit->getType().isMineralField() && unit->getResources() == 0 && find(boulders.begin(), boulders.end(), unit) == boulders.end() && unit->getDistance(playerStartingPosition) < 2560)
	{
		boulders.push_back(unit);
	}
	// If unit not owned by player
	if (unit->getPlayer() == Broodwar->enemy())
	{
		if (unit->getType().isBuilding())
		{
			if (scouting && Broodwar->enemy()->getRace() == Races::Terran && unit->getDistance(getNearestChokepoint(unit->getPosition())->getCenter()) < 256)
			{
				wallIn = true;
			}
			if (enemyBasePositions.size() == 0)
			{
				// Find closest base location to building
				double distance = 5000;
				for (auto base : getStartLocations())
				{
					if (unit->getDistance(base->getPosition()) < distance)
					{
						enemyStartingTilePosition = base->getTilePosition();
						enemyStartingPosition = base->getPosition();
						distance = unit->getDistance(base->getPosition());
					}
				}

				enemyBasePositions.push_back(enemyStartingPosition);
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
	if (unit->getPlayer() == Broodwar->self())
	{
		// Get supply of the unit	
		supply += unit->getType().supplyRequired();
		queuedBuildings.erase(unit->getType());
		if (Broodwar->getFrameCount() >= 100)
		{
			if (unit->getType() == UnitTypes::Enum::Protoss_Nexus)
			{
				allyTerritory.emplace(getRegion(unit->getPosition()));
				forceExpand = 0;
				saturated = false;
			}
		}
	}
}

void McRave::onUnitDestroy(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{
		allyUnits[unit].setDeadFrame(Broodwar->getFrameCount());
		supply -= unit->getType().supplyRequired();
		// For probes, adjust the resource maps to align properly
		if (unit->getType() == UnitTypes::Protoss_Probe)
		{
			if (myGas.find(unit) != myGas.end())
			{
				myGas[myProbes[unit].getTarget()].setGathererCount(myGas[unit].getGathererCount() - 1);
			}
			if (myMinerals.find(unit) != myMinerals.end())
			{
				myMinerals[myProbes[unit].getTarget()].setGathererCount(myMinerals[unit].getGathererCount() - 1);
			}
			myProbes.erase(unit);
		}
		else if (unit->getType() == UnitTypes::Protoss_Nexus)
		{
			if (allyTerritory.find(getRegion(unit->getTilePosition())) != allyTerritory.end())
			{
				allyTerritory.erase(getRegion(unit->getTilePosition()));
			}
		}
	}
	else if (unit->getPlayer() == Broodwar->enemy())
	{
		enemyUnits[unit].setDeadFrame(Broodwar->getFrameCount());
		if (unit->getType().isResourceDepot() && find(enemyBasePositions.begin(), enemyBasePositions.end(), unit->getPosition()) != enemyBasePositions.end())
		{
			enemyBasePositions.erase(find(enemyBasePositions.begin(), enemyBasePositions.end(), unit->getPosition()));
		}
	}
	else if (unit->getType().isMineralField() && unit->getInitialResources() > 0 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) > 0)
	{
		TilePosition closestNexus = Broodwar->getClosestUnit(unit->getPosition(), Filter::IsResourceDepot, 640)->getTilePosition();
		if (closestNexus && find(activeExpansion.begin(), activeExpansion.end(), closestNexus) != activeExpansion.end())
		{
			activeExpansion.erase(find(activeExpansion.begin(), activeExpansion.end(), closestNexus));
			inactiveNexusCnt++;
		}
		for (auto &u : myProbes)
		{
			if (u.second.getTarget() == unit)
			{
				u.second.setTarget(nullptr);
			}
		}
		myMinerals.erase(unit);
	}
}

void McRave::onUnitMorph(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{
		queuedBuildings.erase(unit->getType());
	}
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