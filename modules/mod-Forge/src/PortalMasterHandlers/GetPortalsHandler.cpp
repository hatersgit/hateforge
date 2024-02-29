#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>

class GetPortalsHandler : public ForgeTopicHandler
{
public:
    GetPortalsHandler(ForgeCache* cache, ForgeCommonMessage* messageCommon) : ForgeTopicHandler(ForgeTopic::SEND_PORTALS) {
        fc = cache;
        cm = messageCommon;
    }

    void HandleMessage(ForgeAddonMessage& iam) override {
        std::string msg = iam.message;

        if (iam.message == "cities") {
            
        }
        else if (iam.message == "raids") {
            if (auto rotation = fc->_activeRaidRotation) {
                msg += std::to_string(rotation->raid1Loc->ID) + ";" + std::to_string(rotation->raid2Loc->ID) + ";" + std::to_string(rotation->raid3Loc->ID);
            }
        }
        else if (iam.message == "dungeons") {
            
        }
        else
            return; // do nothing
        
        iam.player->SendForgeUIMsg(ForgeTopic::SEND_PORTALS, msg);
    }

private:
    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
