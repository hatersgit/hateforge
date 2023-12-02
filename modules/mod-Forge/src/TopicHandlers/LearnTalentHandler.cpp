#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "Pet.h"
#include <ForgeCache.cpp>
#include <boost/algorithm/string.hpp>
#include "ForgeCommonMessage.h"

class LearnTalentHandler : public ForgeTopicHandler
{
public:
    LearnTalentHandler(ForgeCache* cache, ForgeCommonMessage* common) : ForgeTopicHandler(ForgeTopic::LEARN_TALENT)
    {
        fc = cache;
        cm = common;
    }

    void HandleMessage(ForgeAddonMessage& iam) override {
        if (iam.message.empty()) // talentType|tabId?rank~rank~rank;*
            return;

        ForgeCharacterSpec* spec;
        if (fc->TryGetCharacterActiveSpec(iam.player, spec)) {
            if (iam.message.size() > 3) {
                auto classinfo = iam.message.substr(0, 3);
                CharacterPointType type = CharacterPointType(base64_char.find_first_of(classinfo.substr(0, 1)));
                auto spec = base64_char.find_first_of(classinfo.substr(1, 2));
                auto pClass = base64_char.find_first_of(classinfo.substr(2, 3));
                if (iam.player->getClass() != pClass) {
                    if (iam.message.size() == 29) {
                        auto ranks = iam.message.substr(3, iam.message.size());

                    }
                    else {
                        iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Malformed talent string.");
                        return;
                    }
                }
                else {
                    iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Incorrect Class in talent string.");
                    return;
                }
            }
            else {
                iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Malformed talent string.");
                return;
            }

            std::vector<std::string> typeSplit;
            boost::algorithm::split(typeSplit, iam.message, boost::is_any_of("|"));
            if (typeSplit.empty() || typeSplit.size() != 2 || !fc->isNumber(typeSplit[0]))
                return;

            auto pointType = static_cast<uint32>(std::stoul(typeSplit[0]));
            if (pointType <= 0 || pointType >= CharacterPointType::PET_TALENT)
                return;

            std::vector<std::string> tabs;
            boost::algorithm::split(tabs, typeSplit[1], boost::is_any_of("*"));
            if (tabs.empty())
                return;

            std::list<ForgeTalentTab*> typeTabs;
            if (fc->TryGetForgeTalentTabs(iam.player, CharacterPointType(pointType), typeTabs)) {
                if (typeTabs.size() != tabs.size())
                    return;

                for (auto tab : tabs) {
                    std::vector<std::string> tabData;
                    boost::algorithm::split(tabData, tab, boost::is_any_of("?"));
                    if (tabData.empty() || tabData.size() != 2 || !fc->isNumber(tabData[0]))
                        return;

                    auto tabId = static_cast<uint32>(std::stoul(tabData[0]));
                    CharacterPointType foundType;
                    ForgeTalentTab* specTab;
                    if (fc->TryGetTabPointType(tabId, foundType) && fc->TryGetTalentTab(iam.player, tabId, specTab)) {
                        if (foundType != pointType)
                            return;

                        if (sConfigMgr->GetBoolDefault("Forge.StrictSpecs", true)) {
                            if (!spec->CharacterSpecTabId) {
                                iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Attempting to learn talents without first selecting a primary spec.");
                                return;
                            }

                            if (tabId != spec->CharacterSpecTabId) {
                                iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Attempting to learn talents outside of selected primary spec.");
                                return;
                            }
                        }

                        auto tabMetaData = fc->_cacheTreeMetaData.find(tabId);
                        if (tabMetaData != fc->_cacheTreeMetaData.end())
                            _treeMetaData = tabMetaData->second;
                        else {
                            iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Unknown tab (" + std::to_string(tabId) + ") in meta data request, report this to staff.");
                            return;
                        }

                        std::vector<std::string> rows;
                        boost::algorithm::split(rows, iam.message, boost::is_any_of(";"));
                        if (rows.empty())
                            return;

                        int i = 0;
                        for (auto row : _treeMetaData->nodes) {
                            if (rows.size() == _treeMetaData->nodes.size()) {
                                std::vector<std::string> cols;
                                boost::algorithm::split(cols, rows[i], boost::is_any_of("~"));
                                if (cols.empty())
                                    return;

                                int j = 0;
                                for (auto col : row.second) {
                                    if (cols.size() == row.second.size()) {
                                        if (!fc->isNumber(cols[j]))
                                            return;

                                        _simplifiedTreeMap[tabId][row.first][col.first] = static_cast<uint32>(std::stoul(cols[j]));
                                        j++;
                                        continue;
                                    }
                                    return;
                                }
                                i++;
                                continue;
                            }
                            return;
                        }

                        ForgeCharacterPoint* points = fc->GetSpecPoints(iam.player, foundType, spec->Id);
                        if (VerifyFlatTable(iam.player, points, specTab)) {
                            fc->ForgetTalents(iam.player, spec, foundType);

                            for (auto ct : toLearn) {
                                auto ft = specTab->Talents[ct->SpellId];
                                bool choiceNode = ct->type == NodeType::CHOICE;

                                spec->Talents[specTab->Id][ct->SpellId] = ct;

                                spec->PointsSpent[specTab->Id] += ft->RankCost * ct->CurrentRank;
                                points->Sum -= ft->RankCost;

                                for (auto s : ft->UnlearnSpells)
                                    iam.player->removeSpell(s, SPEC_MASK_ALL, false);

                                auto rankedSpell = ft->Ranks[ct->CurrentRank];
                                if (choiceNode) {
                                    auto spellInfo = sSpellMgr->GetSpellInfo(ct->SpellId);
                                    if (!spellInfo->HasAttribute(SPELL_ATTR0_PASSIVE))
                                        iam.player->learnSpell(ct->SpellId);
                                    else
                                        iam.player->AddAura(ct->SpellId, iam.player);

                                    uint32 choiceId = fc->GetChoiceNodeFromSpell(ct->SpellId);
                                    spec->ChoiceNodesChosen[choiceId] = ct->SpellId;
                                }
                                else {
                                    auto spellInfo = sSpellMgr->GetSpellInfo(rankedSpell);
                                    if (!spellInfo->HasAttribute(SPELL_ATTR0_PASSIVE))
                                        iam.player->learnSpell(rankedSpell);
                                    else
                                        iam.player->AddAura(rankedSpell, iam.player);
                                }
                            }
                            fc->UpdateCharPoints(iam.player, points);
                        }
                    }
                }

                fc->UpdateCharacterSpec(iam.player, spec);

                cm->SendActiveSpecInfo(iam.player);
                cm->SendSpecInfo(iam.player);
                cm->SendTalents(iam.player, _treeMetaData->TabId);

                iam.player->SendPlaySpellVisual(179); // 53 SpellCastDirected
                iam.player->SendPlaySpellImpact(iam.player->GetGUID(), 362); // 113 EmoteSalute
            }
        }
        else
            iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Attempting to learn talents with an inactive spec.");

    }

    bool VerifyFlatTable(Player* player, ForgeCharacterPoint* points, ForgeTalentTab* tab) {
        toLearn.clear();
        unlocked.clear();
        ForgeCharacterSpec* spec;
        if (fc->TryGetCharacterActiveSpec(player, spec) && tab->ClassMask == player->getClassMask()) {
            std::unordered_map <uint32 /*tabId*/, uint8 /*spent*/> spend;
            for (auto row : _simplifiedTreeMap[tab->Id]) {
                for (auto col : row.second) {
                    if (row.first > _treeMetaData->MaxYDim || col.first > _treeMetaData->MaxXDim)
                        return false;
                    else {
                        auto rowExists = _treeMetaData->nodes.find(row.first);
                        if (rowExists != _treeMetaData->nodes.end()) {
                            auto nodeExists = rowExists->second.find(col.first);
                            if (nodeExists != rowExists->second.end()) {
                                auto node = _treeMetaData->nodes[row.first][col.first];
                                auto findTalent = tab->Talents.find(node->spellId);
                                if (findTalent != tab->Talents.end()) {
                                    ForgeTalent* talent = findTalent->second;

                                    bool choiceNode = talent->nodeType == NodeType::CHOICE;
                                    if (choiceNode && col.second > 0) {
                                        uint32 choiceId = fc->GetChoiceNodeFromSpell(col.second);
                                        if (!choiceId)
                                            return false;
                                    }
                                    else if (talent->Ranks.size() < col.second || col.second < 0)
                                        return false;

                                    if (!talent->Prereqs.empty())
                                        if (std::find(unlocked.begin(), unlocked.end(), node->spellId) == unlocked.end())
                                            return false;

                                    if (talent->RequiredLevel > player->getLevel() || node->pointReq > spend[tab->Id])
                                        return false;

                                    spend[tab->Id] += choiceNode ? talent->RankCost : col.second * talent->RankCost;
                                    if (spend[tab->Id] > points->Max)
                                        return false;

                                    for (auto unlock : node->unlocks)
                                        unlocked.push_back(unlock->spellId);

                                    ForgeCharacterTalent* ct = new ForgeCharacterTalent();
                                    ct->CurrentRank = col.second;
                                    ct->SpellId = choiceNode ? col.second : talent->SpellId;
                                    ct->TabId = tab->Id;
                                    ct->type = talent->nodeType;

                                    toLearn.push_back(ct);
                                }
                            }
                        }
                    }
                }
            }
            return true;
        }
        return false;
    }

private:
    ForgeCommonMessage* cm;
    ForgeCache* fc;

    const std::string base64_char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    ForgeCache::TreeMetaData* _treeMetaData;
    std::unordered_map<uint32 /*tabId*/, std::unordered_map<uint8 /*row*/, std::unordered_map<uint8/*col*/, uint32 /*rank*/>>> _simplifiedTreeMap;
    std::vector<ForgeCharacterTalent*> toLearn = {};
    std::vector<uint32> unlocked = {};
};
