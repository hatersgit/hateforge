#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>
#include <boost/algorithm/string.hpp>

class RerollPerkHandler : public ForgeTopicHandler
{
public:
    RerollPerkHandler(ForgeCache* cache, ForgeCommonMessage* common) : ForgeTopicHandler(ForgeTopic::REROLL_PERK)
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

        if (fc->TryGetCharacterActiveSpec(iam.player, spec)) {

            if (specId != spec->Id) {
                iam.player->SendForgeUIMsg(ForgeTopic::LEARN_PERK_ERROR, "Incorrect spec for reroll request: Id "
                    + std::to_string(specId) + " given when " + std::to_string(spec->Id) + " expected.");
                return;
            }

            if (iam.player->HasItemCount(REROLL_TOKEN)) {
                auto csp = spec->perks.find(spellId);
                if (csp != spec->perks.end()) {
                    iam.player->DestroyItemCount(REROLL_TOKEN, 1, true);
                    auto perk = csp->second;
                    auto rank = perk->rank;
                    auto spell = perk->spell->spellId;

                    iam.player->RemoveAura(spell);
                    rank--;

                    if (rank) {
                        if (auto aura = iam.player->AddAura(spell, iam.player))
                            aura->SetStackAmount(rank);
                        csp->second->rank = rank;
                        spec->perks[spellId] = csp->second;
                        fc->LearnCharacterPerkInternal(iam.player, spec, csp->second);
                    }
                    else {
                        csp->second->rank = 0;
                        fc->LearnCharacterPerkInternal(iam.player, spec, csp->second);
                        spec->perks.erase(spellId);
                    }
                    cm->SendPerks(iam.player, spec->Id);
                }
            }
            else {
                iam.player->SendForgeUIMsg(ForgeTopic::REROLL_PERK_ERROR, "You lack the required item.");
                return;
            }
        }
    }

private:

    ForgeCache* fc;
    ForgeCommonMessage* cm;

    uint32 REROLL_TOKEN = 1000000;
};
