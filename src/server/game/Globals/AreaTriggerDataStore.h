
#ifndef AreaTriggerDataStore_h__
#define AreaTriggerDataStore_h__

#include "Define.h"
#include "AreaTrigger.h"

class AreaTriggerTemplate;
class AreaTriggerMiscTemplate;

struct AreaTriggerData
{
    uint32 id;
    float position_x;
    float position_y;
    float position_z;
    uint32 map_id;
    uint64 spawn_mask;
    uint32 scriptId;
    ObjectGuid::LowType guid;
};

class AreaTriggerDataStore
{
public:
    typedef std::list<AreaTriggerData> AreaTriggerDataList;
    typedef std::unordered_map<uint32, AreaTriggerDataList> AreaTriggerDataContainer;

    void LoadAreaTriggerTemplates();
    void LoadAreaTriggers();

    AreaTriggerTemplate const* GetAreaTriggerTemplate(uint32 areaTriggerId) const;
    AreaTriggerMiscTemplate const* GetAreaTriggerMiscTemplate(uint32 spellMiscValue) const;
    AreaTriggerDataList const* GetStaticAreaTriggersByMap(uint32 map_id) const;

    static AreaTriggerDataStore* Instance();

private:
    AreaTriggerDataContainer _areaTriggerData;
};

#define sAreaTriggerDataStore AreaTriggerDataStore::Instance()

#endif // AreaTriggerDataStore_h__
