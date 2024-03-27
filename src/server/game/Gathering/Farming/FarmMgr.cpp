#include "Farm.h"
#include "FarmMgr.h"
#include "Player.h"

FarmMgr::FarmMgr()
{
    m_UpdateTimer = 0;
}

FarmMgr::~FarmMgr()
{
    _plrFarmMap.clear();
}

FarmMgr* FarmMgr::instance()
{
    static FarmMgr instance;
    return &instance;
}

void FarmMgr::Update(uint32 diff)
{
    m_UpdateTimer += diff;
    for (auto farm : _plrFarmMap)
        farm.second->Update(diff);
}

void FarmMgr::LoadCrops()
{
    uint32 oldMSTime = getMSTime();

    _crops.clear();
    QueryResult result = WorldDatabase.Query("SELECT * FROM crops");

    if (!result)
    {
        LOG_WARN("server.loading", ">> Loaded 0 Crops. DB Table `crops` Is Empty.");
        LOG_INFO("server.loading", " ");
        return;
    }

    uint32 count = 0;

    do
    {
        ++count;

        Field* fields = result->Fetch();
        uint8 id = fields[0].Get<uint32>();
        std::string name = fields[1].Get<std::string>();
        CropBonusGroup group1 = CropBonusGroup(fields[2].Get<uint8>());
        CropBonusGroup group2 = CropBonusGroup(fields[3].Get<uint8>());
        uint32 model = fields[4].Get<uint32>();
        uint32 reward = fields[5].Get<uint32>();

        /*_crops[id] = new Crop();*/
    } while (result->NextRow());

    LOG_INFO("server.loading", ">> Loaded {} Crops in {} ms", count, GetMSTimeDiffToNow(oldMSTime));
    LOG_INFO("server.loading", " ");
}

void FarmMgr::LoadPlayerFarms() {

}
