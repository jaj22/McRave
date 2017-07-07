# McRave - A Broodwar Bot
## For any questions, email christianmccrave@gmail.com
## Bot started 01/03/2017, latest update 05/08/2017

A Broodwar AI Developed in C++ using Visual Studio Express 2013, BWAPI, BWTA and BWEM. This bot is focused on ending the game in the early or middle stages of the game by applying pressure through containing the opponent, using unit efficiency to take as many favorable fights and utilizing the concept of momentum to push advantages. Currently the bot can do the following:

**BWEM Differences:**
- Removed all map drawings
- Applied map instance fix (found by jaj22 on SSCAIT)

**Information:**
- Based on enemy composition, trains an ideal mixture of Dragoons/Zealots.
- Based on enemy build order, creates a tech reaction to counter it.
- Strength based calculations globally for all unique units that we have seen to determine enemys full force.
- Local enemy strength calculations locally around individual units to see if a group of allied units can win the fight against the enemy units around it.
- Uses damage and unit size types plus visible health remaining for local calculations. 
- Detection of enemy upgrades and usage of abilities such as Stim Pack.

**Production:**
- Produces all units and upgrades, dependant on matchup.
- Reserves resources for higher priority units or buildings.
- Prevents buildings being too close to minerals to prevent trapping workers or inefficient mining paths.
- Mineral locking Probes to min/max the Probe count to mining rate ratio.
- Expands when saturated or excessive resources.
- Adds on additional production buildings when none are idle and resources are high.

**Micromanagement:**
- Dragoons will attack and move if the target is melee and too close to the Dragoon or ranged and currently attacking the Dragoon.
- Zealots will attack and move if the target is melee and the Zealot is low health to try to swap enemy units targets onto a healthy Zealot.
- Reavers and High Templars attack the highest cluster of units in an area around it.
- Currently disabled Shuttles.

**Strategy:**
- Based on strength calculations, units decide whether it's a good time to attack or retreat
- If attacking, it's a frontal assault on the enemys main base
- If retreating, it contains the enemy into their base by waiting on the perimeter.
- Contains help in exhausting the enemies resources and slowly cripples the enemies unit count by picking off units that stray too far.

**Future Implementations:**
- Moving fogged units instead of removing their known position.
- Reaver / Shuttle micro
- Harassment micro
