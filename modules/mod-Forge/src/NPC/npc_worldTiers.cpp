#include "Transmogrification.h"
#include "Bag.h"
#include "Common.h"
#include "Config.h"
#include "Creature.h"
#include "DBCStores.h"
#include "DBCStructure.h"
#include "Define.h"
#include "Field.h"
#include "GameEventMgr.h"
#include "GameTime.h"
#include "GossipDef.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Language.h"
#include "Log.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "SpellInfo.h"
#include "Transaction.h"
#include "WorldSession.h"
#include <sstream>
#include <string>

struct WorldTierRangeCheck : public BasicEvent
{
    WorldTierRangeCheck(Player* player, GameObject* go) : guid(go->GetGUID()), player(player)
    {
        ASSERT(player->worldTierNpcCheck == nullptr, "Only one PendingTransmogCheck should be active at one time");
        player->worldTierNpcCheck = this;
        player->m_Events.AddEvent(this, player->m_Events.CalculateTime(500));
    }

    bool Execute(uint64, uint32) override
    {
        GameObject const* go = ObjectAccessor::GetGameObject(*player, guid);
        if (go && go->IsWithinDistInMap(player, go->GetCombatReach() + 5.0f))
        {
            player->m_Events.AddEvent(this, player->m_Events.CalculateTime(500));
            return false;
        }
        
        player->worldTierNpcCheck = nullptr;
        player->SendForgeUIMsg(ForgeTopic::SHOW_WORLD_TIER_SELECT, "0");
        return true;
    }

    ObjectGuid guid;
    Player* player;
    uint32 lastNotSavedAlert = 0;
};

struct PortalMasterRangeCheck : public BasicEvent
{
    PortalMasterRangeCheck(Player* player, Creature* cre) : guid(cre->GetGUID()), player(player)
    {
        ASSERT(player->portalMasterNpcCheck == nullptr, "Only one Portal Master check should be active at one time");
        player->portalMasterNpcCheck = this;
        player->m_Events.AddEvent(this, player->m_Events.CalculateTime(500));
    }

    bool Execute(uint64, uint32) override
    {
        Creature const* cre = ObjectAccessor::GetCreature(*player, guid);
        if (cre && cre->IsWithinDistInMap(player, cre->GetCombatReach() + 5.0f))
        {
            player->m_Events.AddEvent(this, player->m_Events.CalculateTime(500));
            return false;
        }

        player->portalMasterNpcCheck = nullptr;
        player->SendForgeUIMsg(ForgeTopic::SHOW_PORTAL_MASTER_SELECT, "0");
        return true;
    }

    ObjectGuid guid;
    Player* player;
    uint32 lastNotSavedAlert = 0;
};

class go_worldTiers: public GameObjectScript
{
public:
    go_worldTiers() : GameObjectScript("go_worldTiers") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        WorldSession* session = player->GetSession();
        if (!player->worldTierNpcCheck)
            new WorldTierRangeCheck(player, go);

        player->SendForgeUIMsg(ForgeTopic::SHOW_WORLD_TIER_SELECT, "1");
        return true;
    }
};

class npc_portalmaster : public CreatureScript
{
public:
    npc_portalmaster() : CreatureScript("npc_portalmaster") { }

    bool OnGossipHello(Player* player, Creature* cre) override
    {
        WorldSession* session = player->GetSession();
        if (!player->portalMasterNpcCheck)
            new PortalMasterRangeCheck(player, cre);

        player->SendForgeUIMsg(ForgeTopic::SHOW_PORTAL_MASTER_SELECT, "1");
        return true;
    }
};

void AddSC_WorldTiers()
{
    new go_worldTiers();
    new npc_portalmaster();
}
