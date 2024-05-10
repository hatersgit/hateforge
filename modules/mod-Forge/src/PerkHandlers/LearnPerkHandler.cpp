#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include <ForgeCache.cpp>
#include <boost/algorithm/string.hpp>
#include "ForgeCommonMessage.h"

class LearnPerkHandler : public ForgeTopicHandler
{
public:
    LearnPerkHandler(ForgeCache* cache, ForgeCommonMessage*  common) : ForgeTopicHandler(ForgeTopic::LEARN_PERK)
    {
        fc = cache;
        cm = common;
    }

    void HandleMessage(ForgeAddonMessage& iam) override
    {
        if (iam.message.empty())
            return;
        
        std::vector<std::string> results;
        boost::algorithm::split(results, iam.message, boost::is_any_of(";"));

        if (results.empty() || results.size() != 2 || !fc->isNumber(results[0]) || !fc->isNumber(results[1]))
            return;

        uint32 specId = static_cast<uint32>(std::stoul(results[0]));
        uint32 spellId = static_cast<uint32>(std::stoul(results[1]));
        ForgeCharacterSpec* spec;

        if (specId != iam.player->GetActiveSpec()) {
            iam.player->SendForgeUIMsg(ForgeTopic::LEARN_PERK_ERROR, "You are attempting to add a perk to a spec that is not active. Abuse of game systems will result in a ban. Id "
                + std::to_string(specId) + " given when " + std::to_string(iam.player->GetActiveSpec()) + " expected.");
            return;
        }

        if (fc->TryGetCharacterActiveSpec(iam.player, spec))
        {

            if (fc->PerkInQueue(spec, spellId))
            {
                auto perkMap = spec->perks;
                auto perk = perkMap.find(spellId);

                CharacterSpecPerk* csp = new CharacterSpecPerk();
                Perk* spell = fc->GetPerk(spellId);
                if (perk == perkMap.end()) {
                    csp->rank = 0;
                    csp->uuid = spec->perkQueue.begin()->first;
                    csp->spell = spell;
                    perkMap[spellId] = csp;
                }
                else
                    csp = perk->second;

                if (csp->spell->maxRank == csp->rank)
                {
                    iam.player->SendForgeUIMsg(ForgeTopic::LEARN_PERK_ERROR, "This perk cannot be upgraded any further.");
                    return;
                }

                auto spellId = csp->spell->spellId;

                iam.player->RemoveAura(csp->spell->spellId);
                csp->rank++;

                if (auto aura = iam.player->AddAura(spellId, iam.player))
                    aura->SetStackAmount(csp->rank);

                spec->perks[spellId] = csp;
                fc->LearnCharacterPerkInternal(iam.player, spec, csp);
                spec->perkQueue.clear();
                CharacterDatabase.DirectExecute("DELETE FROM character_perk_selection_queue where `guid` = {} and `specId` = {}", iam.player->GetGUID().GetCounter(), spec->Id);

                cm->SendPerks(iam.player, specId);

                iam.player->SendPlaySpellVisual(179); // 53 SpellCastDirected
                iam.player->SendPlaySpellImpact(iam.player->GetGUID(), 362); // 113 EmoteSalute
            }
        }
        else
            iam.player->SendForgeUIMsg(ForgeTopic::LEARN_PERK_ERROR, "The perk you attempted to learn was not offered.");
    }

private:
    ForgeCommonMessage* cm;
    ForgeCache* fc;
};
