#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>
#include <boost/algorithm/string.hpp>

class TakePortalHandler : public ForgeTopicHandler
{
public:
    TakePortalHandler(ForgeCache* cache, ForgeCommonMessage* messageCommon) : ForgeTopicHandler(ForgeTopic::TAKE_PORTAL_TO)
    {
        fc = cache;
        cm = messageCommon;
    }

    void HandleMessage(ForgeAddonMessage& iam) override
    {
        if (iam.message.empty())
            return;

        std::vector<std::string> results;
        boost::algorithm::split(results, iam.message, boost::is_any_of(";"));

        if (results.empty() || results.size() != 2)
            return;

        uint32 id = static_cast<uint32>(std::stoul(results[0]));
        uint32 map = static_cast<uint32>(std::stoul(results[1]));

        if (auto loc = sObjectMgr->GetWorldSafeLoc(map, id))
            iam.player->TeleportTo(loc->Loc);

        return;
    }


private:
    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
