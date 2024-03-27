#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>
#include <boost/algorithm/string.hpp>

class SetWorldTierHandler : public ForgeTopicHandler
{
public:
    SetWorldTierHandler(ForgeCache* cache, ForgeCommonMessage* messageCommon) : ForgeTopicHandler(ForgeTopic::SET_WORLD_TIER)
    {
        fc = cache;
        cm = messageCommon;
    }

    void HandleMessage(ForgeAddonMessage& iam) override
    {
        if (iam.message.empty())
            return;

        if (iam.message == "?") {
            iam.player->SendForgeUIMsg(ForgeTopic::SEND_MAX_WORLD_TIER, std::to_string(std::max(fc->GetCharWorldTierUnlock(iam.player), fc->GetAccountWorldTierUnlock(iam.player))));
            return;
        }

        std::vector<std::string> results;
        boost::algorithm::split(results, iam.message, boost::is_any_of(";"));

        if (results.empty() || results.size() != 2)
            return;

        if (!fc->isNumber(results[0]) || !fc->isNumber(results[1]))
            return;

        uint32 tier = static_cast<uint32>(std::stoul(results[0]));
        uint32 prestige = static_cast<uint32>(std::stoul(results[1]));

        if (tier) {
            if (!iam.player->worldTierNpcCheck && !iam.player->portalMasterNpcCheck)
                return;

            uint8 maxTierAllowed = std::max(fc->GetCharWorldTierUnlock(iam.player), fc->GetAccountWorldTierUnlock(iam.player));
            if (iam.player->GetWorldTier() != tier) {
                if (tier >= WORLD_TIER_1 && tier <= WORLD_TIER_4) {
                    if (tier > iam.player->GetWorldTier() && iam.player->getLevel() < DEFAULT_MAX_LEVEL && !prestige)
                        return;

                    if (maxTierAllowed >= tier)
                        iam.player->SetWorldTier(Difficulty(tier));
                }
            }

            if (prestige) {
                if (!iam.player->worldTierNpcCheck)
                    return;
                ForgeCharacterSpec* spec;
                if (fc->TryGetCharacterActiveSpec(iam.player, spec))
                {
                    for (auto& tab : spec->Talents)
                    {
                        CharacterPointType cpt;
                        if (fc->TryGetTabPointType(tab.first, cpt))

                            switch (cpt)
                            {
                            case CharacterPointType::TALENT_TREE:
                                ForgeTalentTab* ftt;
                                if (fc->TryGetTalentTab(iam.player, tab.first, ftt))
                                {
                                    spec->PointsSpent[ftt->Id] = 0;
                                    for (auto t : tab.second) {

                                        if (auto spellInfo = sSpellMgr->GetSpellInfo(t.second->SpellId)) {
                                            if (spellInfo->HasAttribute(SPELL_ATTR0_PASSIVE))
                                                iam.player->RemoveOwnedAura(ftt->Talents[t.second->SpellId]->Ranks[t.second->CurrentRank]);
                                            else
                                                iam.player->postCheckRemoveSpell(ftt->Talents[t.second->SpellId]->Ranks[t.second->CurrentRank]); // Remove all spells.

                                            t.second->CurrentRank = 0; // only remove talents here.
                                        }
                                    }
                                }
                                break;

                            default:
                                break;
                            }
                    }

                    if (iam.player->getLevel() == DEFAULT_MAX_LEVEL)
                    {
                        fc->PrestigePerks(iam.player);
                        fc->AddCharacterPointsToAllSpecs(iam.player, CharacterPointType::PRESTIGE_COUNT, 1);
                    }
                    else {
                        auto trans = CharacterDatabase.BeginTransaction();
                        trans->Append("delete from character_prestige_perk_carryover where `guid` = {} and specId = {}", iam.player->GetGUID().GetCounter(), spec->Id);
                        trans->Append("delete from character_perk_selection_queue where `guid` = {} and specId = {}", iam.player->GetGUID().GetCounter(), spec->Id);
                        trans->Append("DELETE FROM character_spec_perks WHERE `guid` = {} AND `specId` = {}", iam.player->GetGUID().GetCounter(), spec->Id);

                        CharacterDatabase.CommitTransaction(trans);
                        spec->perks.clear();
                        spec->prestigePerks.clear();
                        spec->perkQueue.clear();
                    }

                    ForgeCharacterPoint* fcp = fc->GetSpecPoints(iam.player, CharacterPointType::TALENT_TREE, spec->Id);
                    ForgeCharacterPoint* baseFcp = fc->GetCommonCharacterPoint(iam.player, CharacterPointType::TALENT_TREE);

                    baseFcp->Sum = 0;
                    fcp->Sum = 0;

                    fc->UpdateCharPoints(iam.player, baseFcp);
                    fc->UpdateCharPoints(iam.player, fcp);
                    fc->UpdateCharacterSpec(iam.player, spec);
                }

                iam.player->ClearBonusTalentPoints();
                iam.player->resetAllSpecs();
                iam.player->SetUInt32Value(PLAYER_XP, 0);
                iam.player->SetLevel(1);
                iam.player->_RemoveAllItemMods();
                for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i) {
                    if (Item* item = iam.player->GetItemByPos(INVENTORY_SLOT_BAG_0, i)) {
                        if (ItemTemplate const* temp = item->GetTemplate()) {
                            if (temp->Quality >= ITEM_QUALITY_UNCOMMON && (temp->Class == ITEM_CLASS_ARMOR || temp->Class == ITEM_CLASS_WEAPON)) {
                                CustomItemTemplate custom = GetItemTemplate(temp->ItemId);
                                custom->AdjustForLevel(iam.player);
                                iam.player->_ApplyItemMods(item, i, true);
                            }
                        }
                    }
                }

                iam.player->UpdateSkillsForLevel();
                iam.player->UpdateAllStats();
                iam.player->SetFullHealth();

                iam.player->ClearQuestStatus(DO_NOT_CLEAR_THESE_QUESTS);
                iam.player->resetSpells();

                iam.player->SetPower(POWER_MANA, iam.player->GetMaxPower(POWER_MANA));
                iam.player->SetPower(POWER_ENERGY, iam.player->GetMaxPower(POWER_ENERGY));
                if (iam.player->GetPower(POWER_RAGE) > iam.player->GetMaxPower(POWER_RAGE))
                    iam.player->SetPower(POWER_RAGE, iam.player->GetMaxPower(POWER_RAGE));
                iam.player->SetPower(POWER_FOCUS, 0);
                iam.player->SetPower(POWER_HAPPINESS, 0);

                iam.player->TeleportTo(fc->GetRaceStartingZone(iam.player->getRace()));
            }
        }
    }

private:
    ForgeCache* fc;
    ForgeCommonMessage* cm;

    std::vector<uint32> DO_NOT_CLEAR_THESE_QUESTS = {90000};
    std::vector<uint32> QUEST_TO_FLAG_AS_COMPLETE = {};
    std::vector<uint32> QUEST_TO_FLAG_AS_ACTIVE = {};
};
