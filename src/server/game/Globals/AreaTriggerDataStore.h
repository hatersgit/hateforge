
#ifndef AreaTriggerDataStore_h__
#define AreaTriggerDataStore_h__

#include "Define.h"
#include "AreaTrigger.h"

class AreaTriggerTemplate;
class AreaTriggerCreateProperties;
struct AreaTriggerId;

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
    void LoadAreaTriggerTemplates();
    void LoadAreaTriggers();

    AreaTriggerTemplate const* GetAreaTriggerTemplate(AreaTriggerId const& areaTriggerId) const;
    AreaTriggerCreateProperties* GetAreaTriggerCreateProperties(uint32 areaTriggerCreatePropertiesId) const;

    static AreaTriggerDataStore* Instance();
private:
};

#define sAreaTriggerDataStore AreaTriggerDataStore::Instance()

#endif // AreaTriggerDataStore_h__
