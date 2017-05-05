// McRave is made by Christian McCrave
// Twitch nicknamed it McRave \o/
// For any questions, email christianmccrave@gmail.com
// Bot started 01/03/2017

// Includes
#include "Header.h"
#include "Global.h"


// Author notes:
// Classes
// Make geometry class, use corner checking and display on grids?

// Static defenses
// Crash testing when losing
// Invis units not being kited 
// Don't train Zealots against T unless speed upgraded

// Zerg don't move out early - Testing
// Boulder removal, heartbreak ridge is an issue - Testing
// If scout dies, no base found - Testing
// Unit class - Testing


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
	//for (auto p : defendHere)
	//{
	//	// Remove blocking mineral patches (maps like Destination)
	//	if (boulders.size() > 0 && Broodwar->getUnitsInRadius(p, 64, Filter::IsMineralField && Filter::Resources == 0).size() > 0)
	//	{
	//		defendHere.erase(find(defendHere.begin(), defendHere.end(), p));
	//		break;
	//	}
	//}
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

#pragma region Ally Grids
	for (auto &u : UnitTracker::Instance().getMyUnits())
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
	for (auto &u : UnitTracker::Instance().getEnUnits())
	{

		// For each enemy unit, add its attack value to each tile it is in range of
		TilePosition unitTilePosition = TilePosition(u.second.getPosition());
		if (u.second.getMaxStrength() > 1.0 && u.second.getDeadFrame() == 0)
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

	UnitTracker::Instance().update();
	UnitTracker::Instance().commandUpdate();
	ProbeTracker::Instance().update();
	StrategyTracker::Instance().update();
	ResourceTracker::Instance().update();
		
	//// If it's a Nexus and we need probes, check if we need probes, then train if needed
	//if (u->getType().isResourceDepot() && u->isIdle() && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) < 60 && (Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + queuedMineral + reservedMineral))
	//{
	//	if (!saturated)
	//	{
	//		u->train(UnitTypes::Protoss_Probe);
	//	}				
	//}
	//// If it's a building capable of production, send to production manager
	//else if (u->getType().isBuilding() && u->getType() != UnitTypes::Protoss_Pylon && u->getType() != UnitTypes::Protoss_Nexus)
	//{
	//	productionManager(u);
	//	continue;
	//}

#pragma region Neutrals
	
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
		if (!b.second.second || !b.second.second->exists())
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
	UnitTracker::Instance().unitDeath(unit);
	if (unit->getPlayer() == Broodwar->self())
	{
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

			if (enemyBasePositions.size() == 0 && unit == scout)
			{
				scouting = false;
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
			}
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