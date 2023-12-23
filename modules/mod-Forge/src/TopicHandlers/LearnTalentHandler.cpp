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

        if (fc->TryGetCharacterActiveSpec(iam.player, spec)) {
            if (iam.message.size() > 3) {
                _treeMetaData.clear();

                auto classinfo = iam.message.substr(0, META_PREFIX);
                CharacterPointType type = CharacterPointType(base64_char.find(classinfo.substr(0, 1))-1);
                auto specId = base64_char.find(classinfo.substr(1, 1));
                auto pClass = base64_char.find(classinfo.substr(2, 1));
                if (iam.player->getClass() == pClass && spec->CharacterSpecTabId == specId) {
                    CharacterPointType foundType;
                    ForgeTalentTab* specTab;
                    if (fc->TryGetTabPointType(specId, foundType) && fc->TryGetTalentTab(iam.player, specId, specTab)) {
                        if (foundType != type)
                            return;

                        auto classMap = fc->_cacheClassNodeToSpell[pClass];
                        auto nodeMap = fc->_cacheSpecNodeToSpell[specId];

                        int treeLen = classMap.size() + nodeMap.size();
                        if (iam.message.size() == treeLen + META_PREFIX) {
                            auto ranks = iam.message.substr(META_PREFIX, iam.message.size());

                            auto tabId = 0;
                            auto spell = 0;
                            for (int c = 0; c < treeLen; c++) {
                                char ch = ranks[c];

                                int treeindex = treeLen - (nodeMap.size());
                                if (c >= treeindex) {
                                    tabId = specId;
                                    spell = nodeMap[c+1-treeindex];
                                }
                                else {
                                    tabId = fc->_cacheClassNodeToClassTree[pClass];
                                    spell = classMap[c + 1];
                                }

                                auto tabMetaData = fc->_cacheTreeMetaData.find(tabId);
                                if (tabMetaData != fc->_cacheTreeMetaData.end())
                                    _treeMetaData[tabId] = tabMetaData->second;
                                else {
                                    iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Unknown tab (" + std::to_string(tabId) + ") in meta data request, report this to staff.");
                                    return;
                                }

                                auto nodeLoc = tabMetaData->second->nodeLocation[spell];

                                _simplifiedTreeMap[tabId][nodeLoc->row][nodeLoc->col] = base64_char.find(ch)-1;
                            }

                            ForgeCharacterPoint* points = fc->GetSpecPoints(iam.player, foundType, spec->Id);
                            if (VerifyFlatTable(iam.player, points, specTab)) {
                                fc->ForgetTalents(iam.player, spec, foundType);
                                std::list<uint32> tabs = {};

                                for (auto ct : toLearn) {
                                    if (std::find(tabs.begin(), tabs.end(), ct->TabId) == tabs.end()) {
                                        spec->Talents[ct->TabId].clear();
                                        tabs.push_back(ct->TabId);
                                    }

                                    bool choiceNode = ct->type == NodeType::CHOICE;
                                    spec->Talents[ct->TabId][ct->SpellId] = ct;
                                    if (ct->CurrentRank > 0) {
                                        ForgeTalentTab* ThisTab;
                                        if (fc->TryGetTalentTab(iam.player, ct->TabId, ThisTab)) {
                                            auto ft = ThisTab->Talents[ct->SpellId];
                                            spec->PointsSpent[ct->TabId] += ft->RankCost * ct->CurrentRank;
                                            points->Sum -= ft->RankCost;

                                            for (auto s : ft->UnlearnSpells)
                                                iam.player->removeSpell(s, SPEC_MASK_ALL, false);

                                            auto rankedSpell = ft->Ranks[ct->CurrentRank];
                                            if (!iam.player->HasSpell(ct->SpellId))
                                                if (choiceNode) {
                                                    iam.player->learnSpell(ct->SpellId);

                                                    uint32 choiceId = fc->GetChoiceNodeFromindex(ct->CurrentRank);
                                                    spec->ChoiceNodesChosen[ct->SpellId] = choiceId;
                                                }
                                                else {
                                                    iam.player->learnSpell(rankedSpell);
                                                }
                                        }
                                    }
                                }

                                iam.player->SetActiveSpec(specId);
                                fc->UpdateCharPoints(iam.player, points);
                                fc->UpdateCharacterSpec(iam.player, spec);

                                cm->SendActiveSpecInfo(iam.player);
                                cm->SendSpecInfo(iam.player);
                                cm->SendTalents(iam.player);

                                iam.player->SendPlaySpellVisual(179); // 53 SpellCastDirected
                                iam.player->SendPlaySpellImpact(iam.player->GetGUID(), 362); // 113 EmoteSalute
                            } else {
                                iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Malformed talent string.");
                                return;
                            }
                        }
                    }
                    else {
                        iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Malformed talent string.");
                        return;
                    }
                }
                else {
                    iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Incorrect Class {" + std::to_string(pClass) + "} or Spec {" + std::to_string(specId) + "} in talent string.");
                    return;
                }
            }
            else {
                iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "Malformed talent string.");
                return;
            }
        }
        else {
            iam.player->SendForgeUIMsg(ForgeTopic::LEARN_TALENT_ERROR, "No active spec.");
            return;
        }
    }

    bool VerifyFlatTable(Player* player, ForgeCharacterPoint* points, ForgeTalentTab* tab) {
        toLearn.clear();
        unlocked.clear();
        if (tab->ClassMask == player->getClassMask()) {
            std::unordered_map <uint32 /*tabId*/, uint8 /*spent*/> spend;
                for (auto tab : _simplifiedTreeMap) {
                    ForgeTalentTab* specTab;
                    if (fc->TryGetTalentTab(player, tab.first, specTab)) {
                        for (auto row : tab.second) {
                            for (auto col : row.second) {
                                if (auto metaData = _treeMetaData[tab.first]) {
                                    if (row.first > metaData->MaxYDim || col.first > metaData->MaxXDim)
                                        return false;
                                    else {
                                        auto rowExists = metaData->nodes.find(row.first);
                                        if (rowExists != metaData->nodes.end()) {
                                            auto nodeExists = rowExists->second.find(col.first);
                                            if (nodeExists != rowExists->second.end()) {
                                                auto node = metaData->nodes[row.first][col.first];
                                                auto findTalent = specTab->Talents.find(node->spellId);
                                                if (findTalent != specTab->Talents.end()) {
                                                    ForgeTalent* talent = findTalent->second;
                                                    bool choiceNode = talent->nodeType == NodeType::CHOICE;
                                                    if (col.second > 0) {
                                                        if (choiceNode) {
                                                            uint32 choiceId = fc->GetChoiceNodeFromindex(col.second);
                                                            if (!choiceId)
                                                                return false;
                                                        }
                                                        else if (talent->Ranks.size() < col.second || col.second < 0)
                                                            return false;

                                                        if (!talent->Prereqs.empty())
                                                            if (std::find(unlocked.begin(), unlocked.end(), node->spellId) == unlocked.end())
                                                                return false;

                                                        if (talent->RequiredLevel > player->getLevel() || node->pointReq > spend[tab.first])
                                                            return false;

                                                        spend[tab.first] += choiceNode ? talent->RankCost : col.second * talent->RankCost;
                                                        if (spend[specTab->Id] > points->Max)
                                                            return false;

                                                        for (auto unlock : node->unlocks)
                                                            unlocked.push_back(unlock->spellId);
                                                    }
                                                    ForgeCharacterTalent* ct = new ForgeCharacterTalent();
                                                    ct->CurrentRank = col.second;
                                                    ct->SpellId = talent->SpellId;
                                                    ct->TabId = tab.first;
                                                    ct->type = talent->nodeType;

                                                    toLearn.push_back(ct);
                                                }
                                            }
                                        }
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

    ForgeCharacterSpec* spec;
    const uint8 META_PREFIX = 3;

    const std::string base64_char = "|ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; // | is to offset string to base 1

    std::unordered_map<uint32, ForgeCache::TreeMetaData*> _treeMetaData;
    std::unordered_map<uint32 /*tabId*/, std::unordered_map<uint8 /*row*/, std::unordered_map<uint8/*col*/, uint32 /*rank*/>>> _simplifiedTreeMap;
    std::vector<ForgeCharacterTalent*> toLearn = {};
    std::vector<uint32> unlocked = {};
};
