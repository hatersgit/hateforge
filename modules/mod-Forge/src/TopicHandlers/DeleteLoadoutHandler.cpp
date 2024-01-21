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
        if (fc->isNumber(iam.message)) {
            uint32 id = static_cast<uint32>(std::stoul(iam.message));
            ForgeCharacterSpec* spec;
            if (fc->TryGetCharacterActiveSpec(iam.player, spec) && id > 1) {
                auto player = fc->_playerTalentLoadouts.find(iam.player->GetGUID().GetCounter());
                if (player != fc->_playerTalentLoadouts.end()) {
                    auto specs = player->second.find(spec->CharacterSpecTabId);
                    if (specs != player->second.end()) {
                        auto found = specs->second.find(id);
                        if (found != specs->second.end()) {
                            specs->second.erase(id);
                            CharacterDatabase.Execute("delete from `forge_character_talent_loadouts` where guid = {} and talentTabId = {} and id = {}", iam.player->GetGUID().GetCounter(), spec->CharacterSpecTabId, id);
                        }
                    }
                }

                cm->SendLoadouts(iam.player);
            }
        }
    }

private:

    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
