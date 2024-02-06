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
    GetSoulShardsHandler(ForgeCache* cache, ForgeCommonMessage* messageCommon) : ForgeTopicHandler(ForgeTopic::GET_TALENTS)
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
            if (fc->TryGetCharacterActiveSpec(iam.player, spec) && iam.player->getLevel() >= 10) {
                cm->SendTalents(iam.player);
            }
        }
    }


private:
    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
