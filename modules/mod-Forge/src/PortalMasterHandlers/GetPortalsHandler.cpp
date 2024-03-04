#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "MapMgr.h"
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
    // WorldSafeLocsEntry* r1 = sObjectMgr->GetWorldSafeLoc(loc1, 0);
    void HandleMessage(ForgeAddonMessage& iam) override {
        std::string msg = "";

        if (!fc->isNumber(iam.message))
            return;

        uint32 id = static_cast<uint32>(std::stoul(iam.message));
        switch (id) {
        case ForgeCache::PortalObjects::RAID: {
            if (auto rotation = fc->_activeRaidRotation) {
                msg += std::to_string(0) + ";" + std::to_string(rotation->raid1Loc)
                    + ";" + sMapStore.LookupEntry(rotation->raid1Loc)->name[sWorld->GetDefaultDbcLocale()] + "*"
                    + std::to_string(0) + ";" + std::to_string(rotation->raid2Loc)
                    + ";" + sMapStore.LookupEntry(rotation->raid2Loc)->name[sWorld->GetDefaultDbcLocale()] + "*"
                    + std::to_string(0) + ";" + std::to_string(rotation->raid3Loc)
                    + ";" + sMapStore.LookupEntry(rotation->raid3Loc)->name[sWorld->GetDefaultDbcLocale()];
            }
        }
                                            break;
        case ForgeCache::PortalObjects::TOWN: {
            auto alliance = iam.player->GetTeamId() == TEAM_ALLIANCE;
            auto cities = alliance ? ALLIANCE_CITIES : HORDE_CITIES;
            std::string delim = "";
            for (auto city : cities) {
                msg += delim + std::to_string(std::get<1>(city)) + ";" + std::to_string(std::get<0>(city)) + ";" + std::get<2>(city);
                delim = "*";
            }
        }
                                            break;
        default:
            return;
        }
        iam.player->SendForgeUIMsg(ForgeTopic::SEND_PORTALS, msg);
    }

private:
    ForgeCache* fc;
    ForgeCommonMessage* cm;

    enum EKLocations
    {
        STORMWIND = 0,
        IRONFORGE = 1,
        BOOTYBAY = 2,
        UNDERCITY = 3,
    };

    enum KalLocations
    {
        DARNASSUS = 0,
        ORGRIMMAR = 1,
        THUNDERBLUFF = 2,
        GADGETZAN = 3,
    };

    enum TBCLocations
    {
        SILVERMOON = 0,
        EXODAR = 1,
    };

    std::vector<std::tuple<uint32, uint32, std::string>> HORDE_CITIES = {
        {0 , BOOTYBAY, "Booty Bay"}, {0, UNDERCITY, "Undercity"}, {1 , ORGRIMMAR, "Orgrimmar"}, {1, THUNDERBLUFF, "Thunderbluff"}, {1 , GADGETZAN, "Gadgetzan"}, {530, SILVERMOON, "Silvermoon City"}
    };

    std::vector<std::tuple<uint32, uint32, std::string>> ALLIANCE_CITIES = {
        {0 , STORMWIND, "Stormwind"}, {0 , IRONFORGE, "Ironforge"}, {0, BOOTYBAY, "Booty Bay"}, {1, DARNASSUS, "Darnassus"}, {1 , GADGETZAN, "Gadgetzan"}, {530, EXODAR, "Exodar"}
    };
};
