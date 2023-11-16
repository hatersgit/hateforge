#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>

class GetTalentsHandler : public ForgeTopicHandler
{
public:
    GetTalentsHandler(ForgeCache* cache, ForgeCommonMessage* messageCommon) : ForgeTopicHandler(ForgeTopic::GET_TALENTS)
    {
        fc = cache;
        cm = messageCommon;

        // Strict spec loading for DF like class specs
        if (sConfigMgr->GetBoolDefault("Forge.StrictSpecs", false)) {

        }
    }

    void HandleMessage(ForgeAddonMessage& iam) override
    {
        if (iam.message == "-1" || iam.message == "" || !fc->isNumber(iam.message))
        {
            ForgeCharacterSpec* spec;
            if (fc->TryGetCharacterActiveSpec(iam.player, spec)) {
                if (sConfigMgr->GetBoolDefault("Forge.StrictSpecs", false) && !spec->CharacterSpecTabId) { // Main spec not selected yet, prompt user
                    auto level10s = fc->_levelClassSpellMap.find(sConfigMgr->GetIntDefault("Forge.StrictSpecs.TalentLevelReq", 10));
                    if (level10s != fc->_levelClassSpellMap.end())
                        specSpells = level10s->second;
                    SendSpecSelectInfo(iam.player);
                }
                else 
                    cm->SendTalents(iam.player);
            }
        }
        else
        {
            uint32 tabId = static_cast<uint32>(std::stoul(iam.message));
            cm->SendTalents(iam.player);
        }
    }


private:
    ForgeCache* fc;
    ForgeCommonMessage* cm;

    std::unordered_map<uint8/*class*/, std::unordered_map<uint32 /*tabId*/, std::vector<uint32 /*spell*/>>> specSpells;

    void SendSpecSelectInfo(Player* player)
    {
        std::list<ForgeTalentTab*> tabs;
        if (fc->TryGetForgeTalentTabs(player, CharacterPointType::TALENT_TREE, tabs))
        {
            int i = 0;
            std::string out = ""; // tabId;iconId;name;description?;spell~spell~spell~mastery?*
            for (auto tab : tabs)
            {
                std::string sep = ";";
                std::string delim = i ? "*" : "";

                out += delim + std::to_string(tab->Id) + sep + tab->SpellIconId + sep
                    + tab->Name + sep + "TODO add descriptions" + sep;

                auto pClass = specSpells.find(player->getClass());
                if (pClass != specSpells.end())
                {
                    auto spec = pClass->second.find(tab->Id);
                    if (spec != pClass->second.end()) {
                        auto spells = spec->second;
                        int j = 0;
                        for (auto spell : spells) {
                            auto spellSep = j ? "~" : "";
                            out += spellSep + std::to_string(spell);
                            j++;
                        }
                    }
                }
                i++;
            }

            player->SendForgeUIMsg(ForgeTopic::PROMPT_CHAR_SPEC, out);
        }
    }
};
