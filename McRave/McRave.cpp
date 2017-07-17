// McRave is made by Christian McCrave
// Twitch nicknamed it McRave \o/
// For any questions, email christianmccrave@gmail.com
// Bot started 01/03/2017

#include "Header.h"
#include "McRave.h"

// --- AUTHOR NOTES ---
// TODO in testing before AIIDE 2017:

// OPTIMIZATION:
// One shot grid updating and minimizing grid iterations - Testing
// Move update of units in StrategyManager to UnitManager - Testing
// Static defense needs a new method

// Edge Cases:
// onUnitMorph - Archons, Eggs, Refineries

// Ready for Testing:
// Test Recall!
// Test size/damage type
// Anti-stone check

// Other:
// Melee unit bouncing - trying min TTT of 2.0 seconds
// Worker pull with FFE
// Cannons stored in buildinginfo defense map
// Use global ally / globaly enemy as local latching
// Archon merging
// Spider mine removal from expansions
// Cannons for each expansion
// Bullets if working, use for unit scoring of performance
// Test FFE against random
// Disable GUI
// Camera center on army center
// Check shuttles

// TODO:
// Remove exists check from target aquisition?
// IsSelected to display information
// Move special units into their own area, separate command manager
// Make target position stuff based on units current command target position
// Move production buildings to the front of the base, tech to the back
// Dijkstras theory for distance grid
// Move stim research to strategy
// Player class to track upgrades/race/supply/strength

// TODO to move to no latency compensation:
// Building idle status stored
// Unit idle status stored?
// Update commands to remove any latency components

void McRaveModule::onStart()
{
	Broodwar->enableFlag(Flag::UserInput);	
	Broodwar->setCommandOptimizationLevel(0);
	Broodwar->setLatCom(true);
	Broodwar->setLocalSpeed(0);
	theMap.Initialize();
	theMap.EnableAutomaticPathAnalysis();
	bool startingLocationsOK = theMap.FindBasesForStartingLocations();
	assert(startingLocationsOK);
	Terrain().onStart();
}

void McRaveModule::onEnd(bool isWinner)
{
}

void McRaveModule::onFrame()
{
	//Players().update();
	Terrain().update();
	Grids().update();
	Resources().update();
	Strategy().update();
	Workers().update();
	Units().update();
	SpecialUnits().update();
	Transport().update();
	Commands().update();
	Buildings().update();
	Production().update();
	BuildOrder().update();
	Bases().update();
	Display().update();
}

void McRaveModule::onSendText(std::string text)
{
	Display().sendText(text);
}

void McRaveModule::onReceiveText(BWAPI::Player player, std::string text)
{
}

void McRaveModule::onPlayerLeft(BWAPI::Player player)
{
	Broodwar->sendText("GG %s!", player->getName().c_str());
}

void McRaveModule::onNukeDetect(BWAPI::Position target)
{
}

void McRaveModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void McRaveModule::onUnitEvade(BWAPI::Unit unit)
{
}

void McRaveModule::onUnitShow(BWAPI::Unit unit)
{
}

void McRaveModule::onUnitHide(BWAPI::Unit unit)
{
}

void McRaveModule::onUnitCreate(BWAPI::Unit unit)
{
	Units().onUnitCreate(unit);
}

void McRaveModule::onUnitDestroy(BWAPI::Unit unit)
{
	Units().removeUnit(unit);
	Terrain().removeTerritory(unit);
}

void McRaveModule::onUnitMorph(BWAPI::Unit unit)
{
	Units().onUnitMorph(unit);
}

void McRaveModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void McRaveModule::onSaveGame(std::string gameName)
{
}

void McRaveModule::onUnitComplete(BWAPI::Unit unit)
{
	Units().onUnitComplete(unit);
}