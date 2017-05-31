﻿// McRave is made by Christian McCrave
// Twitch nicknamed it McRave \o/
// For any questions, email christianmccrave@gmail.com
// Bot started 01/03/2017

// Includes
#include "Header.h"
#include "McRave.h"

// IMPORTANT: Move to BWEM and ditch BWTA, too slow
// DISABLED CURRENTLY: Shuttles, Cannons, DistanceGridHome

// TODOS:
// Storage: buildings

// Move stim research to strategy
// One time supply increase instead of resetting?
// Threatrange grid

// Nexus update TODO:
// Store region
// If cannon was built in region, add to that Nexus

// Resource update TODO:
// Store region
// If region is not ally territory, remove

// Building update TODO:
// Limit building number by region
// Store the closest probe when ready to move to it
// Assign probe to building position
// Probe manager can have a function to check if probe has a building assigned to it and move to it/build it

// Testing:
// If being rushed, move units to mineral line - Testing
// Spider mine removal from expansions - Testing 2.0

void McRave::onStart()
{
	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Set the command optimization level so that common commands can be grouped and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(0);

	Broodwar->setLatCom(true);
	Broodwar->setLocalSpeed(0);
	
	theMap.Initialize();
	theMap.EnableAutomaticPathAnalysis();
	bool startingLocationsOK = theMap.FindBasesForStartingLocations();
	assert(startingLocationsOK);

	BWEM::utils::MapPrinter::Initialize(&theMap);
	BWEM::utils::printMap(theMap);      // will print the map into the file <StarCraftFolder>bwapi-data/map.bmp
	BWEM::utils::pathExample(theMap);   // add to the printed map a path between two starting locations

	if (TerrainTracker::Instance().getAnalyzed() == false) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
	}
	readMap();
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
	TerrainTracker::Instance().update();
	GridTracker::Instance().update();
	ResourceTracker::Instance().update();
	StrategyTracker::Instance().update();
	ProbeTracker::Instance().update();
	UnitTracker::Instance().update();
	TargetTracker::Instance().update();
	SpecialUnitTracker::Instance().update();
	CommandTracker::Instance().update();	
	BuildOrderTracker::Instance().update();
	BuildingTracker::Instance().update();
	ProductionTracker::Instance().update();
	NexusTracker::Instance().update();		
	PylonTracker::Instance().update();
	InterfaceTracker::Instance().update();	
}

void McRave::onSendText(std::string text)
{	
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
}

void McRave::onUnitDiscover(BWAPI::Unit unit)
{	
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
	BuildingTracker::Instance().updateQueue(unit);
}

void McRave::onUnitDestroy(BWAPI::Unit unit)
{
	UnitTracker::Instance().decayUnit(unit);
	SpecialUnitTracker::Instance().removeUnit(unit);
	ProbeTracker::Instance().removeProbe(unit);
	ResourceTracker::Instance().removeResource(unit);
	TerrainTracker::Instance().removeTerritory(unit);
	
	// Ally territory removal
	// Mineral field removal, active expansion removal, inactive count increase	
}

void McRave::onUnitMorph(BWAPI::Unit unit)
{	
	BuildingTracker::Instance().updateQueue(unit);
}

void McRave::onUnitRenegade(BWAPI::Unit unit)
{
}

void McRave::onSaveGame(std::string gameName)
{
}

void McRave::onUnitComplete(BWAPI::Unit unit)
{
}

DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();
	TerrainTracker::Instance().setAnalyzed();	
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