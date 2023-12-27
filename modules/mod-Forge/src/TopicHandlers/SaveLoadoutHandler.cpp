#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>
#include <boost/algorithm/string.hpp>

class SaveLoadoutHandler : public ForgeTopicHandler
{
public:
    SaveLoadoutHandler(ForgeCache* cache, ForgeCommonMessage* fcm) : ForgeTopicHandler(ForgeTopic::SAVE_LOADOUT)
    {
        fc = cache;
        cm = fcm;
    }

    void HandleMessage(ForgeAddonMessage& iam) override
    {
        if (iam.message.empty())
            return;

        std::vector<std::string> results;
        boost::algorithm::split(results, iam.message, boost::is_any_of(";"));

        if (results.empty() || results.size() != 3 || !fc->isNumber(results[0]))
            return;

        uint32 id = static_cast<uint32>(std::stoul(results[0]));
        auto name = results[1];
        auto talentString = results[2];

        auto specId = fc->base64_char.find(talentString.substr(1, 1));
        if (specId != iam.player->GetActiveSpec())
            return;

        auto pClass = fc->base64_char.find(talentString.substr(2, 1));
        if (pClass != iam.player->getClass())
            return;

        auto classMap = fc->_cacheClassNodeToSpell[pClass];
        auto nodeMap = fc->_cacheSpecNodeToSpell[specId];
        int reqLen = classMap.size() + nodeMap.size() + fc->META_PREFIX ;

        if (name.size() > fc->LOADOUT_NAME_MAX || talentString.size() != reqLen)
            return;


        auto plos = fc->_playerTalentLoadouts.find(iam.player->GetGUID().GetCounter());
        if (plos != fc->_playerTalentLoadouts.end())
        {
            auto spec = plos->second.find(specId);
            if (spec != plos->second.end()) {
                auto exists = spec->second.find(id);
                if (exists != spec->second.end()) {
                    exists->second->name = name;
                    exists->second->talentString = talentString;
                }
                CharacterDatabase.Execute("insert into `forge_character_talent_loadouts` (`guid`, `id`, `tabId`, `name`, `talentString`) values ({}, {}, {}, {}, {},) on duplicate key update `name` = {}, `talentString` = {}",
                    iam.player->GetGUID().GetCounter(), id, specId, name, talentString, name, talentString);
            }
        }

        cm->SendLoadouts(iam.player);
    }

private:

    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
