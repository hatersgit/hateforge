/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SC_SCRIPTMGR_H
#define SC_SCRIPTMGR_H

#include "AchievementMgr.h"
#include "ArenaTeam.h"
#include "AuctionHouseMgr.h"
#include "Battleground.h"
#include "Common.h"
#include "DBCStores.h"
#include "DynamicObject.h"
#include "GameEventMgr.h"
#include "Group.h"
#include "InstanceScript.h"
#include "LFGMgr.h"
#include "ObjectMgr.h"
#include "PetDefines.h"
#include "QuestDef.h"
#include "SharedDefines.h"
#include "Tuples.h"
#include "Types.h"
#include "Weather.h"
#include "World.h"
#include <atomic>

// Add support old api modules
#include "AllScriptsObjects.h"

class AuctionHouseObject;
class AuraScript;
class Battleground;
class BattlegroundMap;
class BattlegroundQueue;
class Channel;
class ChatHandler;
class Creature;
class CreatureAI;
class DynamicObject;
class GameObject;
class GameObjectAI;
class GridMap;
class Group;
class Guild;
class InstanceMap;
class InstanceScript;
class Item;
class Map;
class MotionTransport;
class OutdoorPvP;
class Player;
class Quest;
class ScriptMgr;
class Spell;
class SpellCastTargets;
class SpellInfo;
class SpellScript;
class StaticTransport;
class Transport;
class Unit;
class Vehicle;
class WorldObject;
class WorldPacket;
class WorldSocket;
class CharacterCreateInfo;
class SpellScriptLoader;

struct AchievementCriteriaData;
struct AuctionEntry;
struct Condition;
struct ConditionSourceInfo;
struct DungeonProgressionRequirements;
struct GroupQueueInfo;
struct ItemTemplate;
struct OutdoorPvPData;
struct TargetInfo;
struct SpellModifier;

namespace Acore::ChatCommands
{
    struct ChatCommandBuilder;
}

// Check out our guide on how to create new hooks in our wiki! https://www.azerothcore.org/wiki/hooks-script
/*
    TODO: Add more script type classes.

    SessionScript
    CollisionScript
    ArenaTeamScript

*/

#define sScriptMgr ScriptMgr::instance()

template<class TScript>
class ScriptRegistry
{
public:
    typedef std::map<uint32, TScript*> ScriptMap;
    typedef typename ScriptMap::iterator ScriptMapIterator;

    typedef std::vector<TScript*> ScriptVector;
    typedef typename ScriptVector::iterator ScriptVectorIterator;

    // The actual list of scripts. This will be accessed concurrently, so it must not be modified
    // after server startup.
    static ScriptMap ScriptPointerList;
    // After database load scripts
    static ScriptVector ALScripts;

    static void AddScript(TScript* const script)
    {
        ASSERT(script);

        if (!_checkMemory(script))
            return;

        if (script->isAfterLoadScript())
        {
            ALScripts.push_back(script);
        }
        else
        {
            script->checkValidity();

            // We're dealing with a code-only script; just add it.
            ScriptPointerList[_scriptIdCounter++] = script;
            sScriptMgr->IncreaseScriptCount();
        }
    }

    static void AddALScripts()
    {
        for(ScriptVectorIterator it = ALScripts.begin(); it != ALScripts.end(); ++it)
        {
            TScript* const script = *it;

            script->checkValidity();

            if (script->IsDatabaseBound())
            {
                if (!_checkMemory(script))
                {
                    return;
                }

                // Get an ID for the script. An ID only exists if it's a script that is assigned in the database
                // through a script name (or similar).
                uint32 id = sObjectMgr->GetScriptId(script->GetName().c_str());
                if (id)
                {
                    // Try to find an existing script.
                    TScript const* oldScript = nullptr;
                    for (auto iterator = ScriptPointerList.begin(); iterator != ScriptPointerList.end(); ++iterator)
                    {
                        // If the script names match...
                        if (iterator->second->GetName() == script->GetName())
                        {
                            // ... It exists.
                            oldScript = iterator->second;
                            break;
                        }
                    }

                    // If the script is already assigned -> delete it!
                    if (oldScript)
                    {
                        delete oldScript;
                    }

                    // Assign new script!
                    ScriptPointerList[id] = script;

                    // Increment script count only with new scripts
                    if (!oldScript)
                    {
                        sScriptMgr->IncreaseScriptCount();
                    }
                }
                else
                {
                    // The script uses a script name from database, but isn't assigned to anything.
                    if (script->GetName().find("Smart") == std::string::npos)
                        LOG_ERROR("sql.sql", "Script named '{}' is not assigned in the database.",
                                         script->GetName());
                }
            }
            else
            {
                // We're dealing with a code-only script; just add it.
                ScriptPointerList[_scriptIdCounter++] = script;
                sScriptMgr->IncreaseScriptCount();
            }
        }
    }

    // Gets a script by its ID (assigned by ObjectMgr).
    static TScript* GetScriptById(uint32 id)
    {
        ScriptMapIterator it = ScriptPointerList.find(id);
        if (it != ScriptPointerList.end())
            return it->second;

        return nullptr;
    }

private:
    // See if the script is using the same memory as another script. If this happens, it means that
    // someone forgot to allocate new memory for a script.
    static bool _checkMemory(TScript* const script)
    {
        // See if the script is using the same memory as another script. If this happens, it means that
        // someone forgot to allocate new memory for a script.
        for (ScriptMapIterator it = ScriptPointerList.begin(); it != ScriptPointerList.end(); ++it)
        {
            if (it->second == script)
            {
                LOG_ERROR("scripts", "Script '{}' has same memory pointer as '{}'.",
                               script->GetName(), it->second->GetName());

                return false;
            }
        }

        return true;
    }

    // Counter used for code-only scripts.
    static uint32 _scriptIdCounter;
};

// Instantiate static members of ScriptRegistry.
template<class TScript> std::map<uint32, TScript*> ScriptRegistry<TScript>::ScriptPointerList;
template<class TScript> std::vector<TScript*> ScriptRegistry<TScript>::ALScripts;
template<class TScript> uint32 ScriptRegistry<TScript>::_scriptIdCounter = 0;

#endif
