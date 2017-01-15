#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <Windows.h>
#include "BuildingManager.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

DWORD WINAPI AnalyzeThread();
// Remember not to use "Broodwar" in any global class constructor!

class CMProtoBot : public BWAPI::AIModule
{
public:
	
	bool BWTAhandling = false;

	// Unit Variables
	int probeCnt = 0, zealotCnt = 0, dragoonCnt = 0, carrierCnt = 0;	

	// Building Variables
	int queuedMineral, queuedGas = 0;
	int nexusCnt = 0, nexusBuildingCnt = 0, nexusDesired = 0;
	int pylonDesired, pylonBuildingCnt = 0, pylonCnt = 0;
	int gateDesired, gateBuildingCnt = 0, gateCnt = 0;	
	int	gasCnt = 0, gasBuildingCnt = 0, gasDesired = 0;
	int coreCnt = 0, coreBuildingCnt = 0, coreDesired = 0;
	int citadelCnt = 0, citadelBuildingCnt = 0, citadelDesired = 0;
	int forgeCnt = 0, forgeBuildingCnt = 0, forgeDesired = 0;

	int stargateCnt = 0, stargateBuildingCnt = 0, stargateDesired = 0;
	int fleetBeaconCnt = 0, fleetBeaconBuildingCnt = 0, fleetBeaconDesired = 0;

	// Resource IDs
	vector<int> mineralID;
	vector<TilePosition> gasTilePosition;
	vector<int> assimilatorID;

	// Probe ID and their assignments
	int builder = 0, scouter = 0;
	bool scouting = false;
	vector<int> probeID;
	vector<int> gasWorkerID;
	vector<int> mineralWorkerID;
	vector<int> buildingWorkerID;
	vector<int> scoutWorkerID;
	vector<int> deadProbeID;

	// BWTA vectors
	// Base positions and tilepositions, used to find closest bases
	Position nexusPosition;
	vector<Position> basePositions;
	vector<TilePosition> baseTilePositions;
	vector<double> baseDistances;
	vector<double> baseDistancesBuffer;
	vector<double> baseTileDistances;

	vector<double> expansionStartDistance;
	vector<double> expansionRawDistance;
	vector<TilePosition> nextExpansion;
	

	// Variables that are necessary below this line
	BaseLocation* playerStartingLocation;
	Position playerStartingPosition;
	TilePosition playerStartingTilePosition;
	Position enemyStartingPosition;
	TilePosition enemyStartingTilePosition;

	vector<Position>startingLocationPositions;
	Position builderPosition;
	Position scouterPosition;

	set<BWTA::Chokepoint*> myChokes;
	vector<Position>chokepointPositions;
	vector<TilePosition>chokepointTilePositions;
	vector<double> chokepointDistances;
	vector<double> lowestChokepointDistance;
	vector<double> chokepointDistancesBuffer1;

	vector<Position>nearestBasePositions;
	vector<TilePosition>nearestBaseTilePositions;
	vector<TilePosition>nearestBaseTilePositionsBuffer;

	vector<Position>enemyBasePositions;

	vector<double> nearestBases;
	vector<double> furthestBases;

	vector<Position> nearestChokepointPosition;

	// Virtual functions for callbacks, leave these as they are.
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit unit);
	virtual void onUnitEvade(BWAPI::Unit unit);
	virtual void onUnitShow(BWAPI::Unit unit);
	virtual void onUnitHide(BWAPI::Unit unit);
	virtual void onUnitCreate(BWAPI::Unit unit);
	virtual void onUnitDestroy(BWAPI::Unit unit);
	virtual void onUnitMorph(BWAPI::Unit unit);
	virtual void onUnitRenegade(BWAPI::Unit unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit unit);
	// Everything below this line is safe to modify.
	void drawTerrainData();
};


class BWTAVariables
{
	public:
		vector<TilePosition>nearestBaseTilePositions;
};