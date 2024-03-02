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
        auto found = fc->GetPlayerSoulShards(iam.player);
        if (!found.empty()) {
            if (iam.message == "collection") {
                std::string msg = "";
                for (auto shard : found) {
                    if (shard.second->source) {
                        msg += std::to_string(shard.second->source) + "&" + sObjectMgr->GetCreatureTemplate(shard.second->source)->Name + "&" + std::to_string(shard.second->rank) + "&" + std::to_string(shard.second->count) + "&"
                            + std::to_string(shard.second->shard->quality) + "&" + std::to_string(shard.second->shard->special) + "&";
                        auto groupDelim = "";
                        int i = 0;
                        for (auto group : shard.second->shard->groups) {
                            if (i)
                                groupDelim = "$";

                            msg += groupDelim + std::to_string(group);
                            i++;
                        }
                        msg += "~";
                    }
                }
                iam.player->SendForgeUIMsg(ForgeTopic::GET_SOULSHARDS, msg);
            }
            else if (iam.message == "active") {
                fc->SendActiveSoulShards(iam.player);
            }
        }
    }

private:
    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
