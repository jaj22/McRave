# CMProtoBot nicknamed McRave by BWAPI Community \o/
## For any questions, email christianmccrave@gmail.com
## Bot started 01/03/2017, latest update 03/20/2017

A Broodwar AI Developed in C++ using Visual Studio Express 2013, BWAPI, BWTA and BWEM. This bot is focused on ending the game early by applying pressure, using unit efficiency to take as many favorable fights, crippling the opponent slowly and pushing through for a final sweep of their main base. Currently the bot can do the following:

**Information:**
- Based on enemy strategy found through initial scouting, react to any early aggresion or default to a regular mid-game power build
- Initial scout probe stays in enemy base until an enemy unit spawns that isn't a worker
- Enemy strength calculations (globally for all unique units that we have seen)
- Local enemy strength calculations (locally around individual units to see if that unit can win the fight against the units around it)
- Uses damage and unit size types plus visible health remaining for local calculations. 
- Includes detection of enemy upgrades and usage of abilities such as Stim Pack.
- If a Nexus is within a BWTA defined region, it is considered "ally territory".

**Production:**
- Produces all units and upgrades, dependant on matchup.
- Spreads buildings out by at least one tile so units don't get stuck.
- Prevents buildings being too close to minerals to prevent trapping workers or inefficient mining paths
- Mineral locking Probes to min/max the Probe count to mining rate ratio.
- Expands when saturated or excessive resources.

**Micromanagement:**
- Dragoons will attack and move if the target is:
  1. melee and currently too close to the Dragoon.
  2. ranged and currently is currently attacking the Dragoon.
  3. lower ranged unit to outrange and kite away from the Dragoon.
- Zealots will attack and move if the target is melee and the Zealot is low health to try to swap enemy units targets onto a healthy Zealot.
- Reavers and High Templars attack the highest cluster of units in an area around it.
- Currently disabled Shuttles.

**Strategy:**
- Based on strength calculations, units decide whether it's a good time to attack or retreat
- If attacking, it's a frontal assault on the enemys main base
- If retreating, it's a regroup position that is a chokepoint near ally territory.

**Future Implementations:**
- Moving fogged units instead of removing their known position.
- Zerg and Terran build order changes.
