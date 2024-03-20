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

#include "AreaScript.h"
#include "AreaScriptMgr.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "Group.h"
#include "Map.h"
#include "MapMgr.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"

enum AreaScriptAuras {
    CHAMPION    = 62320,
    FANATIC     = 2,
    GHOSTLY     = 3,
    EMPOWERED   = 4,
    BERSERKER   = 5,
    POSSESSED   = 6,

    HOTSPOT_XP  = 1000000,
};

void AreaScript::OnPlayerEnter(Player* player)
{
    if (sAreaScriptMgr->IsHotSpot(_parentZone, _zone)) {
        player->AddAura(HOTSPOT_XP, player);
    }
}

void AreaScript::OnPlayerExit(Player* player)
{
    if (player->HasAura(HOTSPOT_XP))
        player->RemoveAura(HOTSPOT_XP);
}

void AreaScript::HandlePlayerEnterZone(Player* player, uint32 /*zone*/)
{
    _players[player->GetGUID().GetCounter()] = 0.f;
}

void AreaScript::HandlePlayerLeaveZone(Player* player, uint32 /*zone*/)
{
    LOG_DEBUG("AreaScript", "AreaScript: Player {} left an AreaScript zone", player->GetName());
}

bool AreaScript::Update(uint32 diff)
{
    bool objective_changed = false;

    return objective_changed;
}

bool AreaScript::HasPlayer(Player const* player) const
{
    auto found = _players.find(player->GetGUID().GetCounter());
    return found != _players.end() ? true : false;
}

void AreaScript::OnCreatureCreate(Creature* creature)
{
    if (roll_chance_f(100.f)) {
        creature->AddAura(CHAMPION, creature);
    }
}

void AreaScript::OnCreatureRemove(Creature* creature)
{

}

void AreaScript::HandleKill(Player* killer, Unit* killed)
{
    if (auto cre = killed->ToCreature()) {
        if (cre->GetGUID().GetCounter() == boss) {
            progress = 0.f;
            _bossActive = false;

            for (auto entry : cre->GetThreatManager().GetUnsortedThreatList()) {
                if (Player* player = entry->GetVictim()->ToPlayer()) {
                    
                }
            }
        }
        else if (!killed->IsCritter() && !_bossActive) {
            progress++;
            CheckCriteria();
        }
    }
}

void AreaScript::RegisterZone(uint32 zoneId)
{
    sAreaScriptMgr->AddZone(zoneId, this);
}

void AreaScript::SetMapFromZone(uint32 zone)
{
    AreaTableEntry const* areaTable = sAreaTableStore.LookupEntry(zone);
    ASSERT(areaTable);

    Map* map = sMapMgr->CreateBaseMap(areaTable->mapid);
    ASSERT(!map->Instanceable());
    _map = map;
}
