#pragma once
// Include API files
#include <BWAPI.h>
#include "src\bwem.h"
#include "Singleton.h"

// Namespaces
using namespace BWAPI;
using namespace std;

// Include standard libraries that are needed
#include <Windows.h>
#include <set>
#include <ctime>

class UnitInfo;

// Include other source files
#include "BuildingManager.h"
#include "BuildOrder.h"
#include "CommandManager.h"
#include "GridManager.h"
#include "Interface.h"
#include "BaseManager.h"
#include "WorkerManager.h"
#include "ProductionManager.h"
#include "PylonManager.h"
#include "ResourceManager.h"
#include "SpecialUnitManager.h"
#include "StrategyManager.h"
#include "TargetManager.h"
#include "TerrainManager.h"
#include "TransportManager.h"
#include "UnitManager.h"
#include "Util.h"



// Namespace to access all trackers globally
namespace McRave
{	
	inline BuildingTrackerClass& Buildings() { return BuildingTracker::Instance(); }
	inline BuildOrderTrackerClass& BuildOrder() { return BuildOrderTracker::Instance(); }
	inline CommandTrackerClass& Commands() { return CommandTracker::Instance(); }
	inline GridTrackerClass& Grids() { return GridTracker::Instance(); }
	inline InterfaceTrackerClass& Display() { return InterfaceTracker::Instance(); }
	inline BaseTrackerClass& Bases() { return BaseTracker::Instance(); }
	inline ProductionTrackerClass& Production() { return ProductionTracker::Instance(); }
	inline PylonTrackerClass& Pylons() { return PylonTracker::Instance(); }
	inline ResourceTrackerClass& Resources() { return ResourceTracker::Instance(); }
	inline SpecialUnitTrackerClass& SpecialUnits() { return SpecialUnitTracker::Instance(); }
	inline StrategyTrackerClass& Strategy() { return StrategyTracker::Instance(); }
	inline TargetTrackerClass& Targets() { return TargetTracker::Instance(); }
	inline TerrainTrackerClass& Terrain() { return TerrainTracker::Instance(); }
	inline TransportTrackerClass& Transport() { return TransportTracker::Instance(); }
	inline UnitTrackerClass& Units() { return UnitTracker::Instance(); }
	inline UtilTrackerClass& Util() { return UtilTracker::Instance(); }
	inline WorkerTrackerClass& Workers() { return WorkerTracker::Instance(); }
}
using namespace McRave;
