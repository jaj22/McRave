#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class NexusInfo{
private:
	int staticDefenseCount;
	TilePosition staticPosition;
	Unit pylon;
public:
	// Constructors
	NexusInfo();
	~NexusInfo();
	NexusInfo(int, TilePosition, Unit);

	// Accessors
	int getStaticDefenseCount() const;
	TilePosition getStaticPosition() const;
	Unit getPylon() const;

	// Mutators
	void setStaticDefenseCount(int);
	void setStaticPosition(TilePosition);
	void setPylon(Unit);
};