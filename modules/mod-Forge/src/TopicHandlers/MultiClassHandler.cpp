#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>
#include <boost/algorithm/string.hpp>

class MultiClassHandler : public ForgeTopicHandler
{
public:
    MultiClassHandler(ForgeCache* cache, ForgeCommonMessage* cmh) : ForgeTopicHandler(ForgeTopic::MULTICLASS)
    {
        fc = cache;
        cm = cmh;
    }

    void HandleMessage(ForgeAddonMessage& iam) override
    {
        if (iam.message.empty())
            return;

        std::vector<std::string> results;
        boost::algorithm::split(results, iam.message, boost::is_any_of(";"));

        if (results.empty())
            return;

        if (results.size() == 1) {
            if (!fc->isNumber(results[0]))
                return;

            int32 rawOp = static_cast<int32>(std::stoul(results[0]));
            if (rawOp > 2 || rawOp < 0)
                return;

            MultiClassOp op = MultiClassOp(rawOp);
            if (op == MultiClassOp::CLEAR) {
                ForgeCharacterSpec* spec;
                if (fc->TryGetCharacterActiveSpec(iam.player, spec)) {
                    if (auto secondClass = spec->CharacterSpecTabId) {
                        ForgeTalentTab* baseTab;
                        if (fc->TryGetTalentTab(iam.player, secondClass, baseTab)) {
                            auto tab = spec->Talents.find(secondClass);
                            ForgeCharacterPoint* sfp = fc->GetSpecPoints(iam.player, TALENT_TREE, spec->Id);

                            if (tab != spec->Talents.end())
                            {
                                for (auto spell : baseTab->Talents) {
                                    auto found = tab->second.find(spell.first);
                                    if (found != tab->second.end()) {
                                        sfp->Sum += found->second->CurrentRank * spell.second->RankCost;
                                        found->second->CurrentRank = 0;

                                        for (auto rank : spell.second->Ranks) {
                                            if (auto spellInfo = sSpellMgr->GetSpellInfo(rank.second)) {
                                                if (spellInfo->HasEffect(SPELL_EFFECT_LEARN_SPELL))
                                                    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                                                        iam.player->removeSpell(spellInfo->Effects[i].TriggerSpell, SPEC_MASK_ALL, false);

                                                iam.player->removeSpell(rank.second, SPEC_MASK_ALL, false);
                                                iam.player->RemoveAura(rank.second);
                                            }
                                        }
                                    }
                                }
                            }

                            spec->PointsSpent[secondClass] = 0;
                            fc->UpdateCharPoints(iam.player, sfp);

                            spec->CharacterSpecTabId = 0;
                            fc->UpdateCharacterSpecDetailsOnly(iam.player, spec);

                            cm->SendTalentTreeLayout(iam.player);
                            cm->SendSpecInfo(iam.player);
                            return;
                        }
                    }
                }
                else
                    return;
            }
            else
                return;

            return;
        }
        else if (results.size() == 2) {

            if (!fc->isNumber(results[0]) || !fc->isNumber(results[1]))
                return;

            int32 rawOp = static_cast<int32>(std::stoul(results[0]));
            if (rawOp >= MultiClassOp::MAXOP || MultiClassOp::SET < 0)
                return;

            MultiClassOp op = MultiClassOp(rawOp);
            if (op == MultiClassOp::SET) {
                uint32 id = static_cast<uint32>(std::stoul(results[1]));
                ForgeCharacterSpec* currentSpec;
                if (fc->TryGetCharacterActiveSpec(iam.player, currentSpec))
                {
                    ForgeTalentTab* tab;
                    if (fc->TryGetTalentTab(iam.player, id, tab)) {

                        currentSpec->CharacterSpecTabId = id;
                        fc->UpdateCharacterSpecDetailsOnly(iam.player, currentSpec);

                        cm->SendTalentTreeLayout(iam.player);
                        cm->SendSpecInfo(iam.player);
                        cm->SendTalents(iam.player);
                    } else
                        iam.player->SendForgeUIMsg(ForgeTopic::ACTIVATE_SPEC_ERROR, "Unknown Secondary Class");
                }
                else
                    iam.player->SendForgeUIMsg(ForgeTopic::ACTIVATE_SPEC_ERROR, "Somehow, you don't have an active spec.");
            } else
                iam.player->SendForgeUIMsg(ForgeTopic::ACTIVATE_SPEC_ERROR, "Unknown Multiclassing Operation");
        }
    }

private:

    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
