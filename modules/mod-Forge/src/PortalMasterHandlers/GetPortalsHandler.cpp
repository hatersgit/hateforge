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

    void HandleMessage(ForgeAddonMessage& iam) override {
        std::string msg = "";

        if (!fc->isNumber(iam.message))
            return;

        uint32 id = static_cast<uint32>(std::stoul(iam.message));
        switch (id) {
        case ForgeCache::PortalObjects::RAID: {
                if (auto rotation = fc->_activeRaidRotation) {
                    msg += std::to_string(rotation->raid1Loc->ID) + ";" + std::to_string(rotation->raid1Loc->Loc.GetMapId())
                        + ";" + sMapStore.LookupEntry(rotation->raid1Loc->Loc.GetMapId())->name[sWorld->GetDefaultDbcLocale()] + "*"
                        + std::to_string(rotation->raid2Loc->ID) + ";" + std::to_string(rotation->raid2Loc->Loc.GetMapId())
                        + ";" + sMapStore.LookupEntry(rotation->raid1Loc->Loc.GetMapId())->name[sWorld->GetDefaultDbcLocale()] + "*"
                        + std::to_string(rotation->raid3Loc->ID) + ";" + std::to_string(rotation->raid3Loc->Loc.GetMapId())
                        + ";" + sMapStore.LookupEntry(rotation->raid1Loc->Loc.GetMapId())->name[sWorld->GetDefaultDbcLocale()];
                }
            }
            break;
        case ForgeCache::PortalObjects::TOWN: {
                auto alliance = iam.player->GetTeamId() == ALLIANCE;
                auto cities = alliance ? ALLIANCE_CITIES : HORDE_CITIES;
                std::string delim = "";
                for (auto city : cities) {
                    msg += delim + std::to_string(city.second) + ";" + std::to_string(city.first) + ";" + sMapMgr->FindBaseMap(city.second)->GetMapName();
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

    std::vector<std::pair<uint32, uint32>> HORDE_CITIES = {
        {0 , BOOTYBAY}, {0, UNDERCITY}, {1 , ORGRIMMAR}, {1, THUNDERBLUFF}, {1 , GADGETZAN}, {530, SILVERMOON}
    };

    std::vector<std::pair<uint32, uint32>> ALLIANCE_CITIES = {
        {0 , STORMWIND}, {0 , IRONFORGE}, {0, BOOTYBAY}, {1, DARNASSUS}, {1 , GADGETZAN}, {530, EXODAR}
    };
};
