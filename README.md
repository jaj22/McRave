# CMProtoBot nicknamed McRave by BWAPI Community \o/
## For any questions, email christianmccrave@gmail.com
## Bot started 01/03/2017, latest update 24/02/2017

A Broodwar AI Developed in C++ using Visual Basic. This bot is focused on ending the game early by applying pressure, using unit efficiency to take as many favorable fights, crippling the opponent slowly and pushing through for a final sweep of their main base. Currently the bot can do the following:

**Scouts:**
- Based on enemy strategy, react to any early aggresion or default to a regular mid-game power build
- Scout stays in enemy base until a unit spawns
- Enemy strength calculations (globally for all unique units that we have seen)
- Local enemy strength calculations (locally around individual units to see if that unit can win the fight against the units around it)

**Production:**
- Produces high quantity of Dragoons with range upgrade
- Produces reavers and shuttles to transport them and keep them safe in fights
- Techs up to tier 2 units
- Stalls units if needed to build more structures
- Spreads buildings out by at least one tile so units don't get stuck
- Prevents buildings being too close to minerals to prevent trapping workers or inefficient mining paths
- Sets probes to individual mineral fields to maximize mining rate
- Expands after first pressured attack with Shuttle/Reaver combo

**Micro:**
- Dragooons will attack and move if the target is:
  1. melee and currently too close to the Dragoon
  2. ranged and currently is currently attacking the Dragoon
  3. lower ranged unit to outrange and kite away from the Dragoon
- Zealots will attack and move if the target is melee and the Zealot is low health
- Reavers will attack and then load themselves into a shuttle
- Shuttles will drop a Reaver if there is enemies around that the Reaver can attack

**Strategy:**
- Based on strength calculations, units decide whether it's a good time to attack or retreat
- If attacking, it's a frontal assault on the enemys main base
- If retreating, it's a regroup position that is equal to:
  1. the average position of all my units if we dont need to retreat far
  2. the closest nexus if we need to retreat far
