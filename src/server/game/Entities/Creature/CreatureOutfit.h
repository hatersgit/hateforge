#ifndef CREATURE_OUTFIT_H
#define CREATURE_OUTFIT_H

#include "Define.h"
#include "Player.h" // EquipmentSlots
#include "SharedDefines.h" // Gender
#include <memory>

class Creature;
class WorldSession;

class CreatureOutfit
{
public:
    CreatureOutfit(uint8 race, Gender gender);

    uint8 Class = 1;
    uint8 face = 0;
    uint8 skin = 0;
    uint8 hair = 0;
    uint8 facialhair = 0;
    uint8 haircolor = 0;
    std::unordered_map<uint32, uint32> outfitdisplays;
    uint32 npcsoundsid = 0;
    uint64 guild = 0;
    uint32 id = 0;
    uint8 race;
    uint8 gender;
    uint32 displayId;

    CreatureOutfit& SetItemEntry(EquipmentSlots slot, uint32 item_entry);
    CreatureOutfit& SetItemDisplay(EquipmentSlots slot, uint32 displayid)
    {
        outfitdisplays[slot] = displayid;
        return *this;
    }
};

// Remember to change DB query too!
static const uint32 INVIS_MODEL = 11686;
static const uint32 MAX_REAL_MODELID = 0x7FFFFFFF;
static constexpr EquipmentSlots ITEM_SLOTS[] =
{
    EQUIPMENT_SLOT_HEAD,
    EQUIPMENT_SLOT_SHOULDERS,
    EQUIPMENT_SLOT_BODY,
    EQUIPMENT_SLOT_CHEST,
    EQUIPMENT_SLOT_WAIST,
    EQUIPMENT_SLOT_LEGS,
    EQUIPMENT_SLOT_FEET,
    EQUIPMENT_SLOT_WRISTS,
    EQUIPMENT_SLOT_HANDS,
    EQUIPMENT_SLOT_BACK,
    EQUIPMENT_SLOT_TABARD,
};

static bool IsFake(uint32 modelid) { return modelid > MAX_REAL_MODELID; };

#endif
