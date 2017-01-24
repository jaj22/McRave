#include "BuildOrder.h"

using namespace BWAPI;

// Building consistency order: nexus, pylon, gas, gate, forge, core, robo, stargate, citadel, support, fleet, archives, observatory, tribunal
void getBuildOrder()
{	
	pylonDesired = min(22,(int)floor((Broodwar->self()->supplyUsed() / 14)));	
	nexusDesired = min((int)nextExpansion.size(), nexusCnt + (int)floor(probeCnt / (2 * mineralID.size() + 2 * gasTilePosition.size() + 1)));
	switch (Broodwar->enemy()->getRace())
	{
	case Races::Enum::Zerg:
		// Structures
		// Build 1: Counter hydra/ling/lurker using zealot/goon/reaver
		gateDesired = min(2 + nexusCnt, (int)floor(Broodwar->self()->supplyUsed() / 20));
		forgeDesired = min(2, 2 * ((int)floor(Broodwar->self()->supplyUsed() / 160)));
		gasDesired = min((int)gasTilePosition.size(), (int)floor(Broodwar->self()->supplyUsed() / 24));
		coreDesired = min(1, gateCnt);
		roboDesired = min(1, coreCnt);
		supportBayDesired = min(1, roboCnt);
		observatoryDesired = min(1, roboCnt);
		citadelDesired = max(0, nexusCnt - 2);	
		// Build 2: Counter muta/ling using corsairs/zealots
		// Content WIP
		break;
	case Races::Enum::Terran:		
		// Structures
		// Build 1: Counter bio using zealot/goon/DT/carrier
		gateDesired = min(2 + nexusCnt, (int)floor(Broodwar->self()->supplyUsed() / 20));
		forgeDesired = min(2, 2*((int)floor(Broodwar->self()->supplyUsed() / 160)));
		gasDesired = min((int)gasTilePosition.size(), (int)floor(Broodwar->self()->supplyUsed() / 24));
		coreDesired = min(1, gateCnt);
		citadelDesired = min(1, coreCnt);
		archivesDesired = min(1, citadelCnt);
		stargateDesired = min(3, coreCnt*Broodwar->self()->supplyUsed() / 130);
		fleetBeaconDesired = min(1, stargateCnt);
		// Build 2: Counter mech
		// Content WIP
		break;
	case Races::Enum::Protoss:
		// Build 1: Counter gate using zealot/goon/reaver
		gateDesired = min(2 + nexusCnt, 2*(int)floor(Broodwar->self()->supplyUsed() / 20));
		forgeDesired = min(2, 2 * ((int)floor(Broodwar->self()->supplyUsed() / 160)));
		gasDesired = min((int)gasTilePosition.size(), (int)floor(Broodwar->self()->supplyUsed() / 30));
		coreDesired = min(1, gateCnt/2);
		roboDesired = max(0, nexusCnt - 1);
		supportBayDesired = min(1, roboCnt);
		observatoryDesired = min(1, roboCnt);
		citadelDesired = max(0, nexusCnt - 2);
		break;
	}
}
