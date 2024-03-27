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

#ifndef AREA_SCRIPT_H_
#define AREA_SCRIPT_H_

#include "MapMgr.h"
#include "SharedDefines.h"
#include "Util.h"
#include "ZoneScript.h"
#include <array>

class GameObject;

enum AreaScriptAuras {
    CHAMPION    = 9000010,
    EMPOWERED   = 9000011,
    GHOSTLY     = 9000012,
    FANATIC     = 9000013,
    BERSERKER   = 9000014,
    POSSESSED   = 9000015,
    FIERY       = 9000021,

    HOTSPOT_XP  = 1000000,
};

class AreaScript : public ZoneScript
{
    friend class AreaScriptMgr;
public:
    AreaScript(uint32 map, uint32 parentZone, uint32 zone, uint8 goal, uint32 boss)
        : progress(0.f), goal(goal), boss(boss), _zone(zone), _parentZone(parentZone) {

        _map = sMapMgr->FindBaseMap(map);
        _players.clear();
    }

    ~AreaScript() {
        _players.clear();
        progress = 0.f;
    }

    void OnPlayerEnter(Player* /*player*/) override;
    void OnPlayerExit(Player*) override;

    void OnCreatureCreate(Creature* creature) override;
    void OnCreatureRemove(Creature* creature) override;

    void OnCreatureRespawn(Creature* creature) override;

    virtual void HandleKill(Player* /*killer*/, Unit* /*killed*/);

    virtual bool Update(uint32 diff);

    virtual void HandleKillImpl(Player* /*killer*/, Unit* /*killed*/) {}

    uint32 GetTypeId() const { return _typeId; }

    Map* GetMap() const { return _map; }

    void CheckCriteria() {
        if (progress >= goal) {
            // trigger boss spawn
            _bossActive = true;
        }
    }
protected:
    virtual void HandlePlayerEnterZone(Player* player, uint32 zone);
    virtual void HandlePlayerLeaveZone(Player* player, uint32 zone);

    void RegisterZone(uint32 zoneid);
    bool HasPlayer(Player const* player) const;

    // Hack to store map
    void SetMapFromZone(uint32 zone);

    // the map of the objectives belonging to this outdoor pvp
    std::unordered_map<uint32 /*player*/, float /*contribution*/> _players;

private:
    float progress = 0.f;
    float goal;
    uint32 _typeId{};
    bool _sendUpdate{ true };
    Map* _map{};
    uint32 _zone{};
    bool _bossActive{false};
    uint32 _parentZone{};

    uint32 boss = 0;
    std::vector<uint32> _mobAffixes = {EMPOWERED, GHOSTLY, FANATIC, BERSERKER, POSSESSED, FIERY};
};

#endif
