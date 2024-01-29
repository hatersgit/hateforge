/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "Spell.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "Gamemode.h"
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
#include <ApplyTransmogHandler.cpp>
#include <GetTransmogHandler.cpp>
#include <GetTransmogSetsHandler.cpp>
#include <SaveTransmogSetHandler.cpp>
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

#include <unordered_map>
#include <random>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

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
        }
        else {
            fc->AddDefaultLoadout(player);
        }
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
        }/*
        else {
            fc->RemoveTalents(player);
            fc->ApplyTalents(player);
        }*/
        fc->ApplyActivePerks(player);
        LearnSpellsForLevel(player);
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
        player->SetFreeTalentPoints(0);

        ForgeCharacterSpec* spec;
        if (fc->TryGetCharacterActiveSpec(player, spec))
        {
            uint8 currentLevel = player->getLevel();
            uint32 levelMod = fc->GetConfig("levelMod", 2);
            if (oldlevel < currentLevel) {
                int levelDiff = currentLevel - oldlevel;

                //if (currentLevel == fc->GetConfig("MaxLevel", 80))
                //{
                //    fc->AddCharacterPointsToAllSpecs(player, CharacterPointType::PRESTIGE_TREE, fc->GetConfig("PrestigePointsAtMaxLevel", 5));
                //}

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

                    cm->SendActiveSpecInfo(player);
                    cm->SendTalents(player);
                }
                cm->SendSpecInfo(player);
                fc->UpdateCharacterSpec(player, spec);
                LearnSpellsForLevel(player);
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
        OnAddItem(player, item->GetTemplate()->ItemId, count);
    }

    //called after player.additem is called. DO NOT CREATE LOOPS WITH THIS.
    void OnAddItem(Player* player, uint32 item, uint32 count)
    {
        
    }

    void OnCreateItem(Player* player, Item* item, uint32 count) override
    {
        OnAddItem(player, item->GetTemplate()->ItemId, count);
    }

    void OnQuestRewardItem(Player* player, Item* item, uint32 count) override
    {
        OnAddItem(player, item->GetTemplate()->ItemId, count);
    }

    void GenerateItem(Item* item, CustomItemTemplate itemProto, Player const* owner) override
    {
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator

        itemProto->MakeBlankSlate();
        auto e = 2.7183;
        auto invType = itemProto->GetInventoryType();

        float mainStatDistributions[2] = {.50f, .33f};
        ItemModType stats[MAINSTATS] = {ITEM_MOD_STRENGTH, ITEM_MOD_AGILITY, ITEM_MOD_INTELLECT };

        auto slot = fc->_forgeItemSlotValues.find(InventoryType(invType));
        if (slot != fc->_forgeItemSlotValues.end()) {
            float slotmod = slot->second;
            float ilvl = itemProto->GetItemLevel();
            auto qual = itemProto->GetQuality();
            auto formula = 0.f;

            switch (qual) {
            case ITEM_QUALITY_UNCOMMON:
                formula = (1.2f * ilvl + 190.f) / 80.f;
                break;
            case ITEM_QUALITY_RARE:
                formula = (1.2f * ilvl + 140.f) / 60.f;
                break;
            default: // epic+
                formula = (1.2f * ilvl + 94.f) / 40.f;
                break;
            }
            auto itemSlotVal = pow(e, formula);
            auto itemValue = itemSlotVal * slot->second;
            auto curValue = itemValue;

            auto statCount = 2;
            if (itemValue) {
                itemProto->SetBonding(BIND_WHEN_PICKED_UP);
                std::uniform_int_distribution<> mainstatroll(0, 2);
                std::uniform_int_distribution<> mainstatdistroll(0, 1);

                auto mainStat = stats[mainstatroll(gen)];
                while (mainStat == ITEM_MOD_STRENGTH && itemProto->IsArmor() && itemProto->GetSubClass() != ITEM_SUBCLASS_ARMOR_PLATE) {
                    mainStat = stats[mainstatroll(gen)];
                }
                auto statDist = (mainStat == ITEM_MOD_STRENGTH || mainStat == ITEM_MOD_AGILITY) ? mainStatDistributions[mainstatdistroll(gen)] : .33f;
                auto tankDist = statDist == .50f;

                auto amountForStam = (itemValue / 2.f)*statDist;
                auto bonus = tankDist ? 1.15f : 1.f;
                itemProto->SetStatType(1, ITEM_MOD_STAMINA);
                itemProto->SetStatValue(1, (amountForStam / fc->_forgeItemStatValues[ITEM_MOD_STAMINA]) * bonus);
                curValue -= amountForStam;

                auto amountForMainstat = ((itemValue / 2.f) - amountForStam);
                bonus = tankDist ? 1.f : 1.15f;
                itemProto->SetStatsCount(statCount);
                itemProto->SetStatType(0, mainStat);
                itemProto->SetStatValue(0, (amountForMainstat / fc->_forgeItemStatValues[mainStat]) * bonus);
                curValue -= amountForMainstat;

                if (itemProto->IsWeapon()) {
                    float dps = 0.f;
                    switch (itemProto->GetInventoryType()) {
                    case INVTYPE_WEAPON:
                    case INVTYPE_WEAPONMAINHAND:
                    case INVTYPE_WEAPONOFFHAND:
                        dps = 0.711f * itemSlotVal + 4.2f;
                        break;
                    case INVTYPE_2HWEAPON:
                        dps = 0.98f * itemSlotVal + 11.5f;
                        break;
                    case INVTYPE_RANGEDRIGHT:
                        switch (itemProto->GetSubClass()) {
                        case ITEM_SUBCLASS_WEAPON_WAND:
                            dps = 1.28f * itemSlotVal + 5.5f;
                        case ITEM_SUBCLASS_WEAPON_BOW:
                        case ITEM_SUBCLASS_WEAPON_GUN:
                        case ITEM_SUBCLASS_WEAPON_THROWN:
                        case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                            dps = 0.98f * itemSlotVal + 21.5f;
                        }
                        break;
                    default:
                        break;
                    }
                    if (dps) {
                        if (mainStat == ITEM_MOD_INTELLECT) {
                            auto sp = itemSlotVal * 2.435;
                            
                            itemProto->SetStatsCount(statCount);
                            itemProto->SetStatType(statCount, ITEM_MOD_SPELL_POWER);
                            itemProto->SetStatValue(statCount, sp);
                            statCount++;
                            dps -= sp / 4;
                        }
                        std::uniform_int_distribution<> dpsdistr(18, 36);

                        float range = (dps * float(itemProto->GetDelay() / 1000.f)) * 2;
                        float split = range / 2.f;
                        float var = float(dpsdistr(gen) / 100.f) * split;
                        int low = split - var;
                        int top = split + var;

                        itemProto->SetDamageTypeA(SPELL_SCHOOL_NORMAL);
                        itemProto->SetDamageMinA(low);
                        itemProto->SetDamageMaxA(top);
                    }
                    else { return; }
                } else /*armor*/ {
                    switch (itemProto->GetSubClass()) {
                    case ITEM_SUBCLASS_ARMOR_PLATE:
                        itemProto->SetArmor(itemProto->GetItemLevel() * 8.f);
                        break;
                    case ITEM_SUBCLASS_ARMOR_MAIL:
                        itemProto->SetArmor(itemProto->GetItemLevel() * 4.5);
                        break;
                    case ITEM_SUBCLASS_ARMOR_LEATHER:
                        itemProto->SetArmor(itemProto->GetItemLevel() * 2.15);
                        break;
                    case ITEM_SUBCLASS_ARMOR_CLOTH:
                        itemProto->SetArmor(itemProto->GetItemLevel() * 1.08);
                        break;
                    }
                }
                auto statsToRoll = fc->_forgeItemSecondaryStatPools[tankDist && (itemProto->GetSubClass() == ITEM_SUBCLASS_ARMOR_PLATE
                    || itemProto->GetSubClass() == ITEM_SUBCLASS_ARMOR_LEATHER) ? mainStat + ITEM_MOD_STAMINA : mainStat];
                std::uniform_int_distribution<> statdistr(1, statsToRoll.size());
                std::uniform_int_distribution<> secondarydistr(32, 50);
                float secondarySplit = float(secondarydistr(gen)/100.f);
                auto secondaryRolls = qual - 2;

                auto rolled = ITEM_MOD_STAMINA;
                for (int i = 0; i < secondaryRolls;) {
                    auto roll = statdistr(gen) - 1;
                    auto generated = statsToRoll[roll];
                    if (generated != rolled) {
                        if (i > 0) {
                            secondarySplit = 1.f;
                        }
                        auto valueForThis = curValue*secondarySplit;
                        statCount++;
                        auto amount = valueForThis/fc->_forgeItemStatValues[generated];
                        itemProto->SetStatsCount(statCount);
                        itemProto->SetStatType(statCount - 1, generated);
                        itemProto->SetStatValue(statCount - 1, amount);

                        curValue -= valueForThis;
                        rolled = generated;
                        i++;
                    }
                }
                
            }

            itemProto->Save();
            owner->SendItemQueryPacket(&itemProto);
        }
        return ;
    }

    /*void OnGiveXP(Player* player, uint32& amount, Unit* victim) override
    {
        if (Gamemode::HasGameMode(player, GameModeType::CLASSIC))
            return;

        if (player->getLevel() <= 9)
            amount *= fc->GetConfig("Dynamic.XP.Rate.1-9", 2);

        else if (player->getLevel() <= 19)
            amount *= fc->GetConfig("Dynamic.XP.Rate.10-19", 2);

        else if (player->getLevel() <= 29)
            amount *= fc->GetConfig("Dynamic.XP.Rate.20-29", 3);

        else if (player->getLevel() <= 39)
            amount *= fc->GetConfig("Dynamic.XP.Rate.30-39", 3);

        else if (player->getLevel() <= 49)
            amount *= fc->GetConfig("Dynamic.XP.Rate.40-49", 3);

        else if (player->getLevel() <= 59)
            amount *= fc->GetConfig("Dynamic.XP.Rate.50-59", 3);

        else if (player->getLevel() <= 69)
            amount *= fc->GetConfig("Dynamic.XP.Rate.60-69", 4);

        else if (player->getLevel() <= 79)
            amount *= fc->GetConfig("Dynamic.XP.Rate.70-79", 4);
    }*/

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
    sTopicRouter->AddHandler(new GetCollectionsHandler(cache, cm));
    sTopicRouter->AddHandler(new ApplyTransmogHandler(cache, cm));
    sTopicRouter->AddHandler(new SaveTransmogSetHandler(cache, cm));
    sTopicRouter->AddHandler(new GetTransmogSetsHandler(cache, cm));
    sTopicRouter->AddHandler(new GetTransmogHandler(cache, cm));
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

    new ForgeCacheCommands();
}
