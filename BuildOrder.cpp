#include "BuildOrder.h"

using namespace BWAPI;
using namespace std;

void getBuildOrder()
{	
	pylonDesired = min(22,(int)floor((Broodwar->self()->supplyUsed() / 14)));
	nexusDesired = min(5, nexusCnt + (int)floor(((mineralWorkerID.size() + 1)/(2*mineralID.size() + 1))));
	switch (Broodwar->enemy()->getRace())
	{
	case Races::Enum::Zerg:
		// Structures
		// Build is Speedlot Corsair with High Templars/Archons

		// Units

		break;
	case Races::Enum::Terran:		
		// Structures
		// Build is Early Speedlots/Goons -> Add Arbiters (at some point) -> Carriers
		gateDesired = min(2 + nexusCnt, (int)floor(Broodwar->self()->supplyUsed() / 20));
		forgeDesired = min(2, 2*((int)floor(Broodwar->self()->supplyUsed() / 160)));
		gasDesired = min((int)gasTilePosition.size(), (int)floor(Broodwar->self()->supplyUsed() / 24));
		coreDesired = min(1, gateCnt);
		citadelDesired = min(1, coreCnt);
		archivesDesired = min(1, citadelCnt);
		//stargateDesired = min(3, coreCnt*Broodwar->self()->supplyUsed() / 130);
		//fleetBeaconDesired = min(1, stargateCnt);
		//Units

		break;
	case Races::Enum::Protoss:
		// Build is Early Speedlots -> Reavers/Goons (NEED EARLIER GATES, 10,12 GATE)

		// Units

		break;
	}

}









//// Check what structures are desired based on current supplies	
//pylonDesired = (int)floor((Broodwar->self()->supplyUsed() / 14));
//gateDesired = min(3 + nexusCnt, (int)floor(Broodwar->self()->supplyUsed() / 20));
//coreDesired = min(1, (int)floor(Broodwar->self()->supplyUsed() / 36));
//citadelDesired = min(1, nexusCnt - 1);
//roboDesired = min(1, nexusCnt - 1);
////stargateDesired = min(4, nexusCnt*Broodwar->self()->supplyUsed() / 130);
////stargateDesired = min(4, (int)floor(0.00246078*exp(0.0231046*Broodwar->self()->supplyUsed())));
////fleetBeaconDesired = min(1, stargateCnt);
//nexusDesired = min(5, (int)floor(Broodwar->self()->supplyUsed() / 120)) + 1;
//gasDesired = std::min(nexusCnt, (int)floor(Broodwar->self()->supplyUsed() / 34));