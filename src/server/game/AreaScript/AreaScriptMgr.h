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

class Player;
class GameObject;
class Creature;
class ZoneScript;
struct GossipMenuItems;
class AreaScript;

// class to handle player enter / leave / areatrigger / GO use events
class AreaScriptMgr
{
public:
    AreaScriptMgr();
    ~AreaScriptMgr();

    static AreaScriptMgr* instance();
    void AddZone(uint32 zoneid, AreaScript* handle);

    ZoneScript* GetZoneScript(uint32 zoneId);
    AreaScript* GetAreaScript(uint32 areaId);

    void HandlePlayerEnterZone(Player* player, uint32 areaflag);

    void HandlePlayerLeaveZone(Player* player, uint32 areaflag);

    void Update(uint32 diff);

private:
    std::unordered_map<uint32/*zoneId*/, AreaScript*> _scriptMap;
    uint32 m_UpdateTimer;
};

#define sAreaScriptMgr AreaScriptMgr::instance()
