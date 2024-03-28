/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "ChannelMgr.h"
#include "GuildMgr.h"
#include "Guild.h"
#include "Spell.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "Gamemode.h"
#include "CustomItemTemplate.h"
#include <ActivateSpecHandler.cpp>
#include <GetTalentTreeHandler.cpp>
#include <GetCharacterSpecsHandler.cpp>
#include <LearnTalentHandler.cpp>
#include "ForgeCommonMessage.h"
#include <RespecTalentsHandler.cpp>
#include <UnlearnTalentHandler.cpp>
#include <UpdateSpecHandler.cpp>
#include <GetTalentsHandler.cpp>
#include <UseSkillbook.cpp>
#include <unordered_map>
#include <ForgeCache.cpp>
#include <ForgeCacheCommands.cpp>
#include <ActivateClassSpecHandler.cpp>
#include <GetCollectionsHandler.cpp>
 /* #include <ApplyTransmogHandler.cpp>
 #include <GetTransmogHandler.cpp>
 #include <GetTransmogSetsHandler.cpp>
 #include <SaveTransmogSetHandler.cpp> */
#include <StartMythicHandler.cpp>
#include <GetAffixesHandler.cpp>
#include <GetCharacterLoadoutsHandler.cpp>
#include <DeleteLoadoutHandler.cpp>
#include <SaveLoadoutHandler.cpp>

#include <GetPerksHandler.cpp>
#include <GetPerksInspectHandler.cpp>
#include <LearnPerkHandler.cpp>
#include <GetPerkSelectionHandler.cpp>
#include <RerollPerkHandler.cpp>
#include <PrestigeHandler.cpp>

#include <SetWorldTierHandler.cpp>
#include <GetSoulShardsHandler.cpp>
#include <SetSoulShardHandler.cpp>

#include <TakePortalHandler.cpp>
#include <GetPortalsHandler.cpp>

#include <unordered_map>
#include <random>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#define STARTER_GUILD 1

// Add player scripts
class ForgePlayerMessageHandler : public PlayerScript
{
public:
    ForgePlayerMessageHandler(ForgeCache* cache, ForgeCommonMessage* cmh) : PlayerScript("ForgePlayerMessageHandler")
    {
        fc = cache;
        cm = cmh;
    }

    void OnCreate(Player* player) override
    {
        // setup DB
        player->SetSpecsCount(0);
        fc->AddCharacterSpecSlot(player);
        fc->UpdateCharacters(player->GetSession()->GetAccountId(), player);

        if (sConfigMgr->GetBoolDefault("echos", false)) {
            fc->EchosDefaultLoadout(player);
            fc->FillBlankSoulShards(player);
            player->SetWorldTier(WORLD_TIER_1);
        }
        else {
            fc->AddDefaultLoadout(player);
        }
    }

    void OnEquip(Player* player, Item* item, uint8 bag, uint8 slot, bool update) override
    {

    }

    void OnLogin(Player* player) override
    {
        if (!player)
            return;

        LearnSpellsForLevel(player);
        fc->ApplyAccountBoundTalents(player);
        fc->UnlearnFlaggedSpells(player);

        fc->LoadLoadoutActions(player);

        if (fc->IsFlaggedReset(player->GetGUID().GetCounter())) {
            ForgeCharacterSpec* spec;
            if (fc->TryGetCharacterActiveSpec(player, spec))
            {
                ForgeAddonMessage msg;
                msg.topic = "2";
                msg.player = player;
                std::string message = "-1;" + std::to_string(spec->Id);
                msg.message = message;
                sTopicRouter->Route(msg, message);
                fc->ClearResetFlag(player->GetGUID().GetCounter());
            }
        }

        fc->ApplyActivePerks(player);
        LearnSpellsForLevel(player, player->GetLevel());

        if (sConfigMgr->GetBoolDefault("echos", false)) {
            auto foundLoadout = fc->_playerActiveTalentLoadouts.find(player->GetGUID().GetCounter());
            if (foundLoadout == fc->_playerActiveTalentLoadouts.end()) {
                fc->EchosDefaultLoadout(player);
            }

            if (fc->GetCharWorldTierUnlock(player) > fc->GetAccountWorldTierUnlock(player) && fc->GetCharWorldTierUnlock(player) > WORLD_TIER_2)
                fc->SetAccountWorldTierUnlock(player, fc->GetCharWorldTierUnlock(player) - 1);

            player->SendForgeUIMsg(ForgeTopic::SEND_MAX_WORLD_TIER, std::to_string(std::max(fc->GetCharWorldTierUnlock(player), fc->GetAccountWorldTierUnlock(player))));
            fc->RecalculateShardBonuses(player);
        }
    }

    void OnFirstLogin(Player* player) override {
        if (sConfigMgr->GetBoolDefault("StarterGuild.autojoin", false))
            if (Guild* guild = sGuildMgr->GetGuildById(sConfigMgr->GetIntDefault("StarterGuild.id", 0)))
                guild->AddMember(player->GetGUID());
    }

    void BeforeEquip(Player* player, Item* item, uint8 bag, uint8 slot, bool update) override
    {
        if (sConfigMgr->GetBoolDefault("echos", false)) {
            if (auto pProto = item->GetTemplate()) {
                if (pProto->Quality >= ITEM_QUALITY_UNCOMMON && (pProto->Class == ITEM_CLASS_ARMOR || pProto->Class == ITEM_CLASS_WEAPON)
                    && slot != EQUIPMENT_SLOT_BODY && slot != EQUIPMENT_SLOT_TABARD) {
                    CustomItemTemplate custom = GetItemTemplate(pProto->ItemId);
                    custom->AdjustForLevel(player);
                }
            }
        }
    }

    void OnLogout(Player* player) override
    {
        if (!player)
            return;

        ForgeCharacterSpec* spec;
        if (fc->TryGetCharacterActiveSpec(player, spec)) {
            auto active = fc->_playerActiveTalentLoadouts.find(player->GetGUID().GetCounter());
            if (active != fc->_playerActiveTalentLoadouts.end())
                player->SaveLoadoutActions(spec->CharacterSpecTabId, active->second->id);
        }
    }

    void OnDelete(ObjectGuid guid, uint32 accountId) override
    {
        fc->DeleteCharacter(guid, accountId);
        fc->UpdateCharacters(accountId, nullptr);
    }

    // receive message from client
    // since we sent the messag to ourselves the server will not route it back to the player.
    void OnBeforeSendChatMessage(Player* player, uint32& type, uint32& lang, std::string& msg) override
    {
        sTopicRouter->Route(player, type, lang, msg);
    }

    void OnLevelChanged(Player* player, uint8 oldlevel) override
    {

        ForgeCharacterSpec* spec;
        if (fc->TryGetCharacterActiveSpec(player, spec))
        {
            uint8 currentLevel = player->getLevel();
            uint32 levelMod = fc->GetConfig("levelMod", 2);
            if (oldlevel < currentLevel) {
                int levelDiff = currentLevel - oldlevel;

                if (currentLevel >= 10)
                {
                    if (oldlevel < 10 && levelDiff > 1)
                        levelDiff -= (9 - oldlevel);

                    if (levelDiff > 1) {
                        int div = levelDiff / 2;
                        int rem = levelDiff % 2;
                        fc->AddCharacterPointsToAllSpecs(player, CharacterPointType::TALENT_TREE, div);
                        if (rem)
                            div += 1;

                        fc->AddCharacterPointsToAllSpecs(player, CharacterPointType::CLASS_TREE, div);
                    }
                    else {
                        if (currentLevel % 2)
                            fc->AddCharacterPointsToAllSpecs(player, CharacterPointType::TALENT_TREE, 1);
                        else
                            fc->AddCharacterPointsToAllSpecs(player, CharacterPointType::CLASS_TREE, 1);
                    }

                    if (currentLevel > 40 && currentLevel < 61) { // temp tp equalizer until talent redo
                        player->RewardExtraBonusTalentPoints(levelDiff);
                    }

                    cm->SendActiveSpecInfo(player);
                    cm->SendTalents(player);
                }
                cm->SendSpecInfo(player);
                fc->UpdateCharacterSpec(player, spec);
                LearnSpellsForLevel(player, player->GetLevel());

                if (currentLevel == DEFAULT_MAX_LEVEL) {
                    auto tier = player->GetWorldTier();
                    auto unlock = fc->GetCharWorldTierUnlock(player);
                    if (unlock == tier && tier < WORLD_TIER_4) {
                        switch (tier) {
                        case WORLD_TIER_1: {
                            ChatHandler(player->GetSession()).SendSysMessage("|cffBC961CYou have unlocked World Tier 2.");
                            fc->SetCharWorldTierUnlock(player, tier + 1);
                            break;
                        }
                        case WORLD_TIER_2: {
                            ChatHandler(player->GetSession()).SendSysMessage("|cffBC961CYou have unlocked World Tier 3.");
                            fc->SetCharWorldTierUnlock(player, tier + 1);
                            fc->SetAccountWorldTierUnlock(player, tier);
                            break;
                        }
                        case WORLD_TIER_3: {
                            ChatHandler(player->GetSession()).SendSysMessage("|cffBC961CYou have unlocked World Tier 4.");
                            fc->SetCharWorldTierUnlock(player, tier + 1);
                            fc->SetAccountWorldTierUnlock(player, tier);
                            break;
                        }
                        }
                        player->SendForgeUIMsg(ForgeTopic::SEND_MAX_WORLD_TIER, std::to_string(fc->GetCharWorldTierUnlock(player)));
                    }
                    auto rewarded = 1;
                    if (tier > 2) {
                        rewarded++;
                    }
                    player->AddItem(RewardItems::TOKEN_OF_PRESTIGE, rewarded);
                }
            }

            if (sConfigMgr->GetBoolDefault("echos", false)) {
                player->_RemoveAllItemMods();
                for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i) {
                    if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i)) {
                        if (ItemTemplate const* temp = item->GetTemplate()) {
                            if (temp->Quality >= ITEM_QUALITY_UNCOMMON && (temp->Class == ITEM_CLASS_ARMOR || temp->Class == ITEM_CLASS_WEAPON)) {
                                CustomItemTemplate custom = GetItemTemplate(temp->ItemId);
                                custom->AdjustForLevel(player);
                                player->_ApplyItemMods(item, i, true);
                            }
                        }
                    }
                }
                player->SetHealth(player->GetMaxHealth());
            }
        }
    }

    void OnLearnSpell(Player* player, uint32 spellID)
    {
        // check if its forged.
        if (auto* fs = fc->GetTalent(player, spellID))
        {
            if (fs->CurrentRank != 0)
            {
                auto* tab = fc->TalentTabs[fs->TabId];
                auto* spell = tab->Talents[fs->SpellId];
                auto fsId = spell->Ranks[fs->CurrentRank];

                for (auto s : spell->UnlearnSpells)
                    player->removeSpell(s, SPEC_MASK_ALL, false);

                if (!player->HasSpell(fsId))
                    player->learnSpell(fsId);
            }
        }

        fc->LearnExtraSpellsIfAny(player, spellID);
    }

    void OnLootItem(Player* player, Item* item, uint32 count, ObjectGuid lootguid) override
    {
    }

    //called after player.additem is called. DO NOT CREATE LOOPS WITH THIS.
    void OnAddItem(Player* player, uint32 item, uint32 count)
    {

    }

    void OnCreateItem(Player* player, Item* item, uint32 count) override
    {
    }

    void OnQuestRewardItem(Player* player, Item* item, uint32 count) override
    {
    }

    void GenerateItem(CustomItemTemplate* itemProto, Player const* owner) override
    {
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::uniform_int_distribution<> coinflip(0, 1);

        itemProto->MakeBlankSlate();
        itemProto->SetBonding(NO_BIND);
        itemProto->SetStackable(1);

        auto e = 2.7183;
        auto invType = itemProto->GetInventoryType();

        ItemModType stats[MAINSTATS] = { ITEM_MOD_STRENGTH, ITEM_MOD_AGILITY, ITEM_MOD_INTELLECT };

        auto gemSlots = fc->_genItemSlotsForEquipSlot.find(InventoryType(invType));
        if (gemSlots != fc->_genItemSlotsForEquipSlot.end()) {
            std::vector<SocketColor> sockets = {SocketColor::SOCKET_COLOR_BLUE, SocketColor::SOCKET_COLOR_RED, SocketColor::SOCKET_COLOR_YELLOW };
            std::uniform_int_distribution<> socketpicker(0, sockets.size()-1);
            auto slots = gemSlots->second;

            for (int i = 0; i < slots; i++) {
                itemProto->SetSocketColor(i, sockets[socketpicker(gen)]);
            }
        }

        auto slot = fc->_forgeItemSlotValues.find(InventoryType(invType));
        if (slot != fc->_forgeItemSlotValues.end()) {
            float slotmod = slot->second;
            int maxIlvlBase = sConfigMgr->GetIntDefault("WorldTier.base.level", 80);
            int baseIlvl = itemProto->GetItemLevel() > maxIlvlBase ? maxIlvlBase : itemProto->GetItemLevel();

            float ilvl = baseIlvl + uint8(owner->GetWorldTier() - 1) * 10.f;
            itemProto->SetItemLevel(ilvl);
            itemProto->SetRequiredLevel(1);
            itemProto->SetAllowableClass(CLASSMASK_ALL_PLAYABLE);
            auto qual = itemProto->GetQuality();
            auto formula = 0.f;
            auto secondaryRolls = 0;

            switch (qual) {
            case ITEM_QUALITY_UNCOMMON:
                formula = (1.3f * ilvl + 180.f) / 70.f;
                secondaryRolls = 1;
                break;
            case ITEM_QUALITY_RARE:
                formula = (1.3f * ilvl + 180.f) / 65.f;
                secondaryRolls = 2;
                break;
            default: // epic+
                formula = (1.3f * ilvl + 180.f) / 60.f;
                secondaryRolls = 3;
                break;
            }
            auto itemSlotVal = pow(e, formula);
            itemProto->SetItemSlotValue(itemSlotVal);
            auto itemValue = itemSlotVal * slot->second;
            auto curValue = itemValue;

            auto statCount = 2;
            if (itemValue) {

                auto mainStat = itemProto->GenerateMainStatForItem();
                bool tankDist = itemProto->CanRollTank() ? coinflip(gen) ? true : false : false;
                bool canRollStam = itemProto->CanRollStam();

                float amountForAttributes = curValue * .58f;
                if (canRollStam) {
                    float amountForStam = amountForAttributes * .4f;
                    itemProto->SetStatType(statCount - 1, ITEM_MOD_STAMINA);
                    auto amount = amountForStam / fc->_forgeItemStatValues[ITEM_MOD_STAMINA];
                    itemProto->SetStatValue(statCount - 1, amount);
                    itemProto->SetStatValueMax(statCount - 1, amount);
                    amountForAttributes -= amountForStam;
                }

                itemProto->SetStatsCount(statCount);
                itemProto->SetStatType(statCount - 2, mainStat);
                auto amount = amountForAttributes / fc->_forgeItemStatValues[mainStat];
                itemProto->SetStatValue(statCount - 2, amount);
                itemProto->SetStatValueMax(statCount - 2, amount);
                curValue -= amountForAttributes;

                std::vector<ItemModType> rolled = {};
                if (itemProto->IsWeapon()) {
                    std::uniform_int_distribution<> dpsdistr(18, 36);
                    std::uniform_real<> delaydistr(.8, 1.2);
                    
                    itemProto->SetDelay(uint32(itemProto->GetDelay()*float(delaydistr(gen))));

                    float dps = itemProto->CalculateDps();
                    if (dps) {
                        if (mainStat == ITEM_MOD_INTELLECT && (itemProto->IsWeapon() && itemProto->GetSubClass() != ITEM_SUBCLASS_WEAPON_WAND)) {
                            auto sp = itemValue * 2.435;

                            itemProto->SetStatsCount(statCount);
                            itemProto->SetStatType(statCount, ITEM_MOD_SPELL_POWER);
                            itemProto->SetStatValue(statCount, sp);
                            itemProto->SetStatValueMax(statCount, sp);
                            statCount++;
                            dps -= sp / 4;

                            rolled.push_back(ITEM_MOD_SPELL_POWER);
                        }

                        float range = (dps * float(itemProto->GetDelay() / 1000.f)) * 2;
                        float split = range / 2.f;
                        float var = float(dpsdistr(gen) / 100.f) * split;
                        int low = split - var;
                        int top = split + var;

                        itemProto->SetDamageMinA(low);
                        itemProto->SetMaxDamageMinA(low);
                        itemProto->SetDamageMaxA(top);
                        itemProto->SetMaxDamageMaxA(top);
                    }
                    else { return; }
                }
                else /*armor*/ {
                    switch (itemProto->GetSubClass()) {
                    case ITEM_SUBCLASS_ARMOR_PLATE: {
                        auto baseArmor = itemProto->GetItemLevel() * 7.6f * slotmod;
                        itemProto->SetArmor(int32(baseArmor));

                        if (secondaryRolls) {
                            auto valueForThis = curValue / 3;
                            auto amountForTankStat = valueForThis / fc->_forgeItemStatValues[ITEM_MOD_DEFENSE_SKILL_RATING];
                            statCount++;
                            itemProto->SetStatsCount(statCount);
                            itemProto->SetStatType(statCount - 1, ITEM_MOD_DEFENSE_SKILL_RATING);
                            itemProto->SetStatValue(statCount - 1, amountForTankStat);
                            itemProto->SetStatValueMax(statCount - 1, amountForTankStat);
                            secondaryRolls--;
                            curValue -= valueForThis;
                            rolled.push_back(ITEM_MOD_DEFENSE_SKILL_RATING);
                        }
                        break;
                    }
                    case ITEM_SUBCLASS_ARMOR_MAIL: {
                        itemProto->SetArmor(itemProto->GetItemLevel() * 2.5f * slotmod);

                        if (secondaryRolls) {
                            auto valueForThis = curValue / 3;
                            auto splitval = valueForThis / 2;
                            auto amountForAP = splitval / fc->_forgeItemStatValues[ITEM_MOD_ATTACK_POWER];
                            statCount++;
                            itemProto->SetStatsCount(statCount);
                            itemProto->SetStatType(statCount - 1, ITEM_MOD_ATTACK_POWER);
                            itemProto->SetStatValue(statCount - 1, amountForAP);
                            itemProto->SetStatValueMax(statCount - 1, amountForAP);

                            auto amountForSP = splitval / fc->_forgeItemStatValues[ITEM_MOD_SPELL_POWER];
                            statCount++;
                            itemProto->SetStatsCount(statCount);
                            itemProto->SetStatType(statCount - 1, ITEM_MOD_SPELL_POWER);
                            itemProto->SetStatValue(statCount - 1, amountForSP);
                            itemProto->SetStatValueMax(statCount - 1, amountForSP);

                            secondaryRolls--;
                            curValue -= valueForThis;
                            rolled.push_back(ITEM_MOD_ATTACK_POWER);
                            rolled.push_back(ITEM_MOD_SPELL_POWER);
                        }
                        break;
                    }
                    case ITEM_SUBCLASS_ARMOR_LEATHER: {
                        itemProto->SetArmor(itemProto->GetItemLevel() * 2.5f * slotmod);

                        if (secondaryRolls) {
                            auto valueForThis = curValue / 3;
                            auto amountForAP = valueForThis / fc->_forgeItemStatValues[ITEM_MOD_ATTACK_POWER];
                            statCount++;
                            itemProto->SetStatsCount(statCount);
                            itemProto->SetStatType(statCount - 1, ITEM_MOD_ATTACK_POWER);
                            itemProto->SetStatValue(statCount - 1, amountForAP);
                            itemProto->SetStatValueMax(statCount - 1, amountForAP);
                            secondaryRolls--;
                            curValue -= valueForThis;
                            rolled.push_back(ITEM_MOD_ATTACK_POWER);
                        }
                        break;
                    }
                    case ITEM_SUBCLASS_ARMOR_CLOTH: {
                        itemProto->SetArmor(itemProto->GetItemLevel() * 2.5f * slotmod);

                        if (secondaryRolls) {
                            auto valueForThis = curValue / 3;
                            auto amountForSP = valueForThis / fc->_forgeItemStatValues[ITEM_MOD_SPELL_POWER];
                            statCount++;
                            itemProto->SetStatsCount(statCount);
                            itemProto->SetStatType(statCount - 1, ITEM_MOD_SPELL_POWER);
                            itemProto->SetStatValue(statCount - 1, amountForSP);
                            itemProto->SetStatValueMax(statCount - 1, amountForSP);
                            secondaryRolls--;
                            curValue -= valueForThis;
                            rolled.push_back(ITEM_MOD_SPELL_POWER);
                        }
                        break;
                    }
                    case ITEM_SUBCLASS_ARMOR_SHIELD: {
                        itemProto->SetArmor(itemProto->GetItemLevel() * 22.5f * slotmod);
                        itemProto->SetBlock(1.f * ilvl);
                        auto amountForBR = (curValue / 3) / fc->_forgeItemStatValues[ITEM_MOD_BLOCK_RATING];
                        statCount++;
                        itemProto->SetStatsCount(statCount);
                        itemProto->SetStatType(statCount - 1, ITEM_MOD_BLOCK_RATING);
                        itemProto->SetStatValue(statCount - 1, amountForBR);
                        itemProto->SetStatValueMax(statCount - 1, amountForBR);
                        secondaryRolls--;
                        curValue -= amountForBR;
                        rolled.push_back(ITEM_MOD_BLOCK_RATING);
                    }}
                }

                std::uniform_int_distribution<> secondarydistr(0, 8);

                for (int i = 0; i < secondaryRolls;) {
                    auto rolledTank = tankDist ? coinflip(gen) ? ITEM_MOD_STAMINA : mainStat : mainStat;
                    auto statsToRoll = fc->_forgeItemSecondaryStatPools[rolledTank];
                    std::uniform_int_distribution<> statdistr(1, statsToRoll.size());

                    auto roll = statdistr(gen) - 1;
                    auto generated = statsToRoll[roll];
                    if (std::find(rolled.begin(), rolled.end(), generated) == rolled.end()) {
                        float split = secondaryRolls > 1 ? secondarydistr(gen) : 0;
                        auto valueForThis = curValue / (secondaryRolls - i) * (1.f + split / 100.f);
                        auto amount = valueForThis / fc->_forgeItemStatValues[generated];

                        statCount++;
                        itemProto->SetStatsCount(statCount);
                        itemProto->SetStatType(statCount - 1, generated);
                        itemProto->SetStatValue(statCount - 1, amount);
                        itemProto->SetStatValueMax(statCount - 1, amount);

                        curValue -= valueForThis;
                        rolled.push_back(generated);
                        i++;
                    }
                }
            }
        }
        itemProto->InitializeQueryData();
        itemProto->Save();
        owner->SendItemQueryPacket(itemProto);
        return;
    }

    void OnGiveXP(Player* player, uint32& amount, Unit* victim, uint8 source) override
    {
        if (player->getLevel() <= 9)
            amount *= fc->GetConfig("Dynamic.XP.Rate.1-9", 10);

        else if (player->getLevel() <= 19)
            amount *= fc->GetConfig("Dynamic.XP.Rate.10-19", 10);

        else if (player->getLevel() <= 29)
            amount *= fc->GetConfig("Dynamic.XP.Rate.20-29", 13);

        else if (player->getLevel() <= 39)
            amount *= fc->GetConfig("Dynamic.XP.Rate.30-39", 13);

        else if (player->getLevel() <= 49)
            amount *= fc->GetConfig("Dynamic.XP.Rate.40-49", 15);

        else if (player->getLevel() <= 59)
            amount *= fc->GetConfig("Dynamic.XP.Rate.50-59", 15);

        /*else if (player->getLevel() <= 69)
            amount *= fc->GetConfig("Dynamic.XP.Rate.60-69", 4);

        else if (player->getLevel() <= 79)
            amount *= fc->GetConfig("Dynamic.XP.Rate.70-79", 4);*/
    }

    void OnCreatureKill(Player* killer, Creature* killed) override
    {
        if (Group* group = killer->GetGroup()) {
            group->DoForAllMembers([this, killed](Player* player) {
                if (player->IsAtLootRewardDistance(killed) && player->isAllowedToLoot(killed))
                    RollSoulForPlayer(player, killed); });
        } else {
            RollSoulForPlayer(killer, killed);
        }
    }

    void RollSoulForPlayer(Player* killer, Creature* killed) {
        if (killed->GetCreatureType() < CREATURE_TYPE_NOT_SPECIFIED && killed->GetCreatureType() != CREATURE_TYPE_CRITTER) {
            auto chance = 1.f;
            if (killed->isElite())
                chance = 5.f;
            if (killed->isWorldBoss())
                chance = 10.f;

            if (roll_chance_f(chance))
            {
                WorldPacket data;
                LocaleConstant locale = killer->GetSession()->GetSessionDbLocaleIndex();
                std::string msg = killed->GetName() + " has been denied peace in death, their soul is now yours.";
                ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_BOSS_WHISPER, LANG_UNIVERSAL, killed, killer, msg, 0, "", locale);
                killer->SendDirectMessage(&data);

                if (auto source = killed->GetCreatureTemplate()) {
                    if (auto shard = fc->GetSoulShard(source->Entry)) {
                        fc->HandleSoulShard(killer, source->Entry);
                    }
                    else {
                        fc->CreateSoulShardFor(killed->GetCreatureTemplate());
                        fc->HandleSoulShard(killer, source->Entry);
                    }
                    cm->SendSoulShards(killer);
                }
            }
        }
    }

private:
    TopicRouter* Router;
    ForgeCache* fc;
    ForgeCommonMessage* cm;

    uint8 GetPrestigeStatus(Player* player)
    {
        ForgeCharacterPoint* presCp = fc->GetCommonCharacterPoint(player, CharacterPointType::PRESTIGE_COUNT);
        return presCp->Sum;
    }

    void LearnSpellsForLevel(Player* player) {
        if (player->HasUnitState(UNIT_STATE_DIED))
            player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

        auto pClass = fc->_levelClassSpellMap.find(player->getClass());
        if (pClass != fc->_levelClassSpellMap.end()) {
            for (auto race : pClass->second) {
                if (player->getRaceMask() & race.first) {
                    for (auto level : race.second) {
                        if (level.first <= player->getLevel()) {
                            for (auto spell : level.second) {
                                if (player->HasSpell(spell))
                                    continue;

                                player->learnSpell(spell);
                            }
                        }
                    }
                }
            }
        }
    }

    // Functions for spells that piggyback trainers: to be deprecated
    void LearnSpellsForLevel(Player* player, uint8 newlevel) {

        // remove fake death
        if (player->HasUnitState(UNIT_STATE_DIED))
            player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

        auto spells = sObjectMgr->getTrainerSpellsForClass();
        if (!spells.empty()) {
            LearnSpellsForClass(player, CLASS_DRUID + 3, newlevel, spells);
            LearnSpellsForClass(player, player->getClass(), newlevel, spells);
        }
    }

    void LearnSpellsForClass(Player* player, uint8 search, uint8 newLevel, CacheTrainerSpellByClassContainer pool) {
        auto classSpells = pool.find(search);
        if (classSpells != pool.end()) {
            for (auto spell : classSpells->second)
            {
                TrainerSpell const* tSpell = &spell.second;

                if (player->HasSpell(tSpell->spell))
                    continue;

                bool valid = true;
                for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                {
                    if (!tSpell->learnedSpell[i])
                        continue;
                    if (!player->IsSpellFitByClassAndRace(tSpell->learnedSpell[i]))
                    {
                        valid = false;
                        break;
                    }
                }

                if (newLevel < tSpell->reqLevel)
                    continue;

                if (!valid)
                    continue;

                if (player->GetTrainerSpellState(tSpell) != TRAINER_SPELL_GREEN)
                    continue;

                // learn explicitly or cast explicitly
                if (tSpell->IsCastable())
                    player->CastSpell(player, tSpell->spell, true);
                else
                    player->learnSpell(tSpell->spell);
            }
        }
    }
};

// Add all scripts in one
void AddForgePlayerMessageHandler()
{
    ForgeCache* cache = ForgeCache::instance();
    ForgeCommonMessage* cm = new ForgeCommonMessage(cache);

    new ForgePlayerMessageHandler(cache, cm);
    sTopicRouter->AddHandler(new ActivateSpecHandler(cache, cm));
    sTopicRouter->AddHandler(new GetTalentsHandler(cache, cm));
    sTopicRouter->AddHandler(new GetCharacterSpecsHandler(cache, cm));
    sTopicRouter->AddHandler(new GetTalentTreeHandler(cache, cm));
    sTopicRouter->AddHandler(new LearnTalentHandler(cache, cm));
    sTopicRouter->AddHandler(new UnlearnTalentHandler(cache, cm));
    sTopicRouter->AddHandler(new RespecTalentsHandler(cache, cm));
    sTopicRouter->AddHandler(new UpdateSpecHandler(cache));
    sTopicRouter->AddHandler(new PrestigeHandler(cache, cm));
    sTopicRouter->AddHandler(new ActivateClassSpecHandler(cache, cm));
    /*  sTopicRouter->AddHandler(new GetCollectionsHandler(cache, cm));
        sTopicRouter->AddHandler(new ApplyTransmogHandler(cache, cm));
        sTopicRouter->AddHandler(new SaveTransmogSetHandler(cache, cm));
        sTopicRouter->AddHandler(new GetTransmogSetsHandler(cache, cm));
        sTopicRouter->AddHandler(new GetTransmogHandler(cache, cm)); */
    sTopicRouter->AddHandler(new StartMythicHandler(cache, cm));
    sTopicRouter->AddHandler(new GetAffixesHandler(cache, cm));
    sTopicRouter->AddHandler(new GetCharacterLoadoutsHandler(cache, cm));
    sTopicRouter->AddHandler(new DeleteLoadoutHandler(cache, cm));
    sTopicRouter->AddHandler(new SaveLoadoutHandler(cache, cm));

    sTopicRouter->AddHandler(new GetPerksHandler(cache, cm));
    sTopicRouter->AddHandler(new LearnPerkHandler(cache, cm));
    sTopicRouter->AddHandler(new GetPerkSelectionHandler(cache, cm));
    sTopicRouter->AddHandler(new RerollPerkHandler(cache, cm));
    sTopicRouter->AddHandler(new GetPerksInspectHandler(cache, cm));

    sTopicRouter->AddHandler(new SetWorldTierHandler(cache, cm));
    sTopicRouter->AddHandler(new GetSoulShardsHandler(cache, cm));
    sTopicRouter->AddHandler(new SetSoulShardHandler(cache, cm));

    sTopicRouter->AddHandler(new TakePortalHandler(cache, cm));
    sTopicRouter->AddHandler(new GetPortalsHandler(cache, cm));

    new ForgeCacheCommands();
}
