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

enum AreaScriptUpdateIntervals {
    AREA_SCRIPT_UPDATE = 1000,
    AREA_SCRIPT_HOT_SPOT_TIMER = 360000,
};

#include "AreaScript.h"
#include "AreaScriptMgr.h"
#include "Player.h"
#include "Zones/BattlefieldWG.h"

AreaScriptMgr::AreaScriptMgr()
{
    m_UpdateTimer = 0;
    FillNewHotSpots();
}

AreaScriptMgr::~AreaScriptMgr()
{
    _scriptMap.clear();
    _hotSpots.clear();
}

AreaScriptMgr* AreaScriptMgr::instance()
{
    static AreaScriptMgr instance;
    return &instance;
}

void AreaScriptMgr::AddZone(uint32 zoneid, AreaScript* handle)
{
    _scriptMap[zoneid] = handle;
}

void AreaScriptMgr::Update(uint32 diff)
{
    m_UpdateTimer += diff;
    if (m_UpdateTimer > AREA_SCRIPT_UPDATE)
    {
        for (auto script : _scriptMap)
            script.second->Update(m_UpdateTimer);
        m_UpdateTimer = 0;
    }
    m_hotSpotTimer += diff;
    if (m_hotSpotTimer > AREA_SCRIPT_HOT_SPOT_TIMER || _hotSpots.empty()) {
        _hotSpots.clear();
        FillNewHotSpots();
    }

}

void AreaScriptMgr::HandlePlayerEnterZone(Player* player, uint32 zoneid)
{
    if (auto script = GetZoneScript(zoneid)) {
        script->OnPlayerEnter(player);
    }
    else return;
}

void AreaScriptMgr::HandlePlayerLeaveZone(Player* player, uint32 zoneid)
{
    if (auto script = GetZoneScript(zoneid)) {
        script->OnPlayerExit(player);
    }
    else return;
}

ZoneScript* AreaScriptMgr::GetZoneScript(uint32 zoneId)
{
    auto found = _scriptMap.find(zoneId);
    return found != _scriptMap.end() ? found->second : nullptr;
}

AreaScript* AreaScriptMgr::GetAreaScript(uint32 zoneId)
{
    auto found = _scriptMap.find(zoneId);
    return found != _scriptMap.end() ? found->second : nullptr;
}
