#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>

class GetCharacterLoadoutsHandler : public ForgeTopicHandler
{
public:
    GetCharacterLoadoutsHandler(ForgeCache* cache, ForgeCommonMessage* fcm) : ForgeTopicHandler(ForgeTopic::GET_LOADOUTS)
    {
        fc = cache;
        cm = fcm;
    }

    void HandleMessage(ForgeAddonMessage& iam) override
    {
        cm->SendLoadouts(iam.player);
    }

private:

    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
