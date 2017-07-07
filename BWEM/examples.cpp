//////////////////////////////////////////////////////////////////////////
//
// This file is part of the BWEM Library.
// BWEM is free software, licensed under the MIT/X11 License. 
// A copy of the license is provided with the library in the LICENSE file.
// Copyright (c) 2015, 2017, Igor Dimitrijevic
//
//////////////////////////////////////////////////////////////////////////


#include "examples.h"
#include "map.h"
#include "base.h"
#include "neutral.h"
#include "mapDrawer.h"
#include "gridMap.h"
#include "bwapiExt.h"

using namespace BWAPI;
using namespace BWAPI::UnitTypes::Enum;
namespace { auto & bw = Broodwar; }

using namespace std;


namespace BWEM {

using namespace utils;
using namespace BWAPI_ext;

namespace utils
{




void drawMap(const Map & theMap)
{
	if (MapDrawer::showFrontier)
		for (auto f : theMap.RawFrontier())
			bw->drawBoxMap(Position(f.second), Position(f.second + 1), MapDrawer::Color::frontier, bool("isSolid"));

	for (int y = 0 ; y < theMap.Size().y ; ++y)
	for (int x = 0 ; x < theMap.Size().x ; ++x)
	{
		TilePosition t(x, y);
		const Tile & tile = theMap.GetTile(t, check_t::no_check);
		
		if (MapDrawer::showUnbuildable && !tile.Buildable())
			drawDiagonalCrossMap(Position(t), Position(t + 1), MapDrawer::Color::unbuildable);
		
		if (MapDrawer::showGroundHeight && (tile.GroundHeight() > 0))
		{
			auto col = tile.GroundHeight() == 1 ? MapDrawer::Color::highGround : MapDrawer::Color::veryHighGround;
			bw->drawBoxMap(Position(t), Position(t)+6, col, bool("isSolid"));
			if (tile.Doodad()) bw->drawTriangleMap(center(t)+Position(0, 5), center(t)+Position(-3, 2), center(t)+Position(+3, 2), Colors::White);
		}
	}

	for (int y = 0 ; y < theMap.WalkSize().y ; ++y)
	for (int x = 0 ; x < theMap.WalkSize().x ; ++x)
	{
		WalkPosition w(x, y);
		const MiniTile & miniTile = theMap.GetMiniTile(w, check_t::no_check);
			
		if (MapDrawer::showSeas && miniTile.Sea())
			drawDiagonalCrossMap(Position(w), Position(w + 1), MapDrawer::Color::sea);
		
		if (MapDrawer::showLakes && miniTile.Lake())
			drawDiagonalCrossMap(Position(w), Position(w + 1), MapDrawer::Color::lakes);
	}

	if (MapDrawer::showCP)
		for (const Area & area : theMap.Areas())
			for (const ChokePoint * cp : area.ChokePoints())
				for (ChokePoint::node end : {ChokePoint::end1, ChokePoint::end2})
					bw->drawLineMap(Position(cp->Pos(ChokePoint::middle)), Position(cp->Pos(end)), MapDrawer::Color::cp);

	if (MapDrawer::showMinerals)
		for (auto & m : theMap.Minerals())
		{
			bw->drawBoxMap(Position(m->TopLeft()), Position(m->TopLeft() + m->Size()) , MapDrawer::Color::minerals);
			if (m->Blocking())
				drawDiagonalCrossMap(Position(m->TopLeft()), Position(m->TopLeft() + m->Size()), MapDrawer::Color::minerals);
		}

	if (MapDrawer::showGeysers)
		for (auto & g : theMap.Geysers())
			bw->drawBoxMap(Position(g->TopLeft()), Position(g->TopLeft() + g->Size()) , MapDrawer::Color::geysers);

	if (MapDrawer::showStaticBuildings)
		for (auto & s : theMap.StaticBuildings())
		{
			bw->drawBoxMap(Position(s->TopLeft()), Position(s->TopLeft() + s->Size()) , MapDrawer::Color::staticBuildings);
			if (s->Blocking())
				drawDiagonalCrossMap(Position(s->TopLeft()), Position(s->TopLeft() + s->Size()), MapDrawer::Color::staticBuildings);
		}

	for (const Area & area : theMap.Areas())
		for (const Base & base : area.Bases())
		{
			if (MapDrawer::showBases)
				bw->drawBoxMap(Position(base.Location()), Position(base.Location() + UnitType(Terran_Command_Center).tileSize()) , MapDrawer::Color::bases);

			if (MapDrawer::showAssignedRessources)
			{
				vector<Ressource *> AssignedRessources(base.Minerals().begin(), base.Minerals().end());
				AssignedRessources.insert(AssignedRessources.end(), base.Geysers().begin(), base.Geysers().end());

				for (const Ressource * r : AssignedRessources)
					bw->drawLineMap(base.Center(), r->Pos(), MapDrawer::Color::assignedRessources);
			}
		}
}

struct SimpleGridMapCell
{
	vector<Unit>		Units;
};



class SimpleGridMap : public GridMap<SimpleGridMapCell, 8>
{
public:
						SimpleGridMap(const Map * pMap) : GridMap(pMap) {}

	void				Add(Unit unit);
	void				Remove(Unit unit);

	vector<Unit>		GetUnits(TilePosition topLeft, TilePosition bottomRight, Player player) const;
};


void SimpleGridMap::Add(Unit unit)
{
	auto & List = GetCell(TilePosition(unit->getPosition())).Units;

	if (!contains(List, unit)) List.push_back(unit);
}


void SimpleGridMap::Remove(Unit unit)
{
	auto & List = GetCell(TilePosition(unit->getPosition())).Units;

	really_remove(List, unit);
}


vector<Unit> SimpleGridMap::GetUnits(TilePosition topLeft, TilePosition bottomRight, Player player) const
{
	vector<Unit> Res;

	int i1, j1, i2, j2;
	tie(i1, j1) = GetCellCoords(topLeft);
	tie(i2, j2) = GetCellCoords(bottomRight);

	for (int j = j1 ; j <= j2 ; ++j)
	for (int i = i1 ; i <= i2 ; ++i)
		for (Unit unit : GetCell(i, j).Units)
			if (unit->getPlayer() == player)
				if (BWAPI_ext::inBoundingBox(TilePosition(unit->getPosition()), topLeft, bottomRight))
					Res.push_back(unit);

	return Res;
}


void gridMapExample(const Map & theMap)
{

	// 1) Initialization
	SimpleGridMap Grid(&theMap);

	//  Note: generally, you will create one instance of GridMap, after calling Map::Instance().Initialize().
	
	
	// 2) Update (in AIModule::onFrame)
	for (int j = 0 ; j < Grid.Height() ; ++j)
	for (int i = 0 ; i < Grid.Width() ; ++i)
		Grid.GetCell(i, j).Units.clear();

	for (Unit unit : Broodwar->getAllUnits())
		Grid.Add(unit);

	//  Note: alternatively, you could use the Remove and Add methods only, in the relevant BWAPI::AIModule methods.


	// 3) Use
	TilePosition centerTile(theMap.Center());
	for (Unit unit : Grid.GetUnits(centerTile-10, centerTile+10, Broodwar->self()))
		Broodwar << "My " << unit->getType().getName() << " #" << unit->getID() << " is near the center of the map." << endl;
}



}} // namespace BWEM::utils



