#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>

class DeleteLoadoutHandler : public ForgeTopicHandler
{
public:
    DeleteLoadoutHandler(ForgeCache* cache, ForgeCommonMessage* fcm) : ForgeTopicHandler(ForgeTopic::DELETE_LOADOUT)
    {
        fc = cache;
        cm = fcm;
    }

    void HandleMessage(ForgeAddonMessage& iam) override
    {
        if(fc->isNumber(iam.message)) {
            uint32 id = static_cast<uint32>(std::stoul(iam.message));
            auto found = fc->_playerTalentLoadouts.find(id);
            if (found != fc->_playerTalentLoadouts.end()) {
                fc->_playerTalentLoadouts.erase(id);
                CharacterDatabase.Execute("delete from `forge_character_talent_loadouts` where guid = {} and id = {}", iam.player->GetGUID().GetCounter(), id);
            }

            cm->SendLoadouts(iam.player);
        }
    }

private:

    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
