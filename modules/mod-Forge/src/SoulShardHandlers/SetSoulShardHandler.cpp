#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>
#include <boost/algorithm/string.hpp>

class SetSoulShardHandler : public ForgeTopicHandler
{
public:
    SetSoulShardHandler(ForgeCache* cache, ForgeCommonMessage* messageCommon) : ForgeTopicHandler(ForgeTopic::SET_SOULSHARDS)
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

        uint32 slot = static_cast<uint32>(std::stoul(results[0]));
        uint32 shard = static_cast<uint32>(std::stoul(results[1]));

        if (slot > MAX_ACTIVE_SHARDS)
            return;

        if (auto soulshard = fc->PlayerHasSoulShard(iam.player, shard))
            if (!fc->PlayerHasSoulShardActive(iam.player, shard))
                fc->SetSoulShardSlotFor(soulshard, slot, iam.player);

        return;
    }


private:
    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
