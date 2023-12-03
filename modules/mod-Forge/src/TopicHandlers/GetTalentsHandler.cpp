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
};
