#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>

class GetSoulShardsHandler : public ForgeTopicHandler
{
public:
    GetSoulShardsHandler(ForgeCache* cache, ForgeCommonMessage* messageCommon) : ForgeTopicHandler(ForgeTopic::GET_SOULSHARDS) {
        fc = cache;
        cm = messageCommon;
    }

    void HandleMessage(ForgeAddonMessage& iam) override {
        if (iam.message == "collection") {
            cm->SendSoulShards(iam.player);
        }
        else if (iam.message == "active") {
            fc->SendActiveSoulShards(iam.player);
        }

    }

private:
    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
