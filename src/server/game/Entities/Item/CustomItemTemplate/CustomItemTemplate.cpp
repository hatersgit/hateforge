/*
 * This file is part of tswow (https://github.com/tswow/).
 * Copyright (C) 2020 tswow <https://github.com/tswow/>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "CustomItemTemplate.h"
#include "ObjectMgr.h"
#include "Player.h"
#include <memory.h>
// #if TRINITY
// #include "QueryPackets.h"
// #endif

CustomItemTemplate::CustomItemTemplate(ItemTemplate const* _info)
    : info(const_cast<ItemTemplate*>(_info))
{}

CustomItemTemplate::CustomItemTemplate(ItemTemplate * _info)
    : info(_info)
{}


CustomItemTemplate::CustomItemTemplate()
    : info(nullptr)
{}

uint32 CustomItemTemplate::GetEntry() { return info->ItemId; };
void CustomItemTemplate::SetEntry(uint32 value) {
    info->ItemId = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetClass() { return info->Class; };
void CustomItemTemplate::SetClass(uint32 value) {
    info->Class = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetSubClass() { return info->SubClass; };
void  CustomItemTemplate::SetSubClass(uint32 value) {
    info->SubClass = value;
    info->m_isDirty = true;
};
int32 CustomItemTemplate::GetSoundOverrideSubclass() { return info->SoundOverrideSubclass; };
void  CustomItemTemplate::SetSoundOverrideSubclass(int32 value) {
    info->SoundOverrideSubclass = value;
    info->m_isDirty = true;
};
std::string CustomItemTemplate::GetName() { return info->Name1; };
void CustomItemTemplate::SetName(std::string const& name) {
    info->Name1 = name;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetDisplayInfoID() { return info->DisplayInfoID; };
void CustomItemTemplate::SetDisplayInfoID(uint32 value) {
    info->DisplayInfoID = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetQuality() { return info->Quality; };
void CustomItemTemplate::SetQuality(uint32 value) {
    info->Quality = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetFlags() { return info->Flags; };
void CustomItemTemplate::SetFlags(uint32 value) {
    info->Flags = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetFlags2() { return info->Flags2; };
void CustomItemTemplate::SetFlags2(uint32 value) {
    info->Flags2 = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetBuyCount() { return info->BuyCount; };
void CustomItemTemplate::SetBuyCount(uint32 value) {
    info->BuyCount = value;
    info->m_isDirty = true;
};
int32 CustomItemTemplate::GetBuyPrice() { return info->BuyPrice; };
void  CustomItemTemplate::SetBuyPrice(int32 value) {
    info->BuyPrice = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetSellPrice() { return info->SellPrice; };
void CustomItemTemplate::SetSellPrice(uint32 value) {
    info->SellPrice = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetInventoryType() { return info->InventoryType; };
void CustomItemTemplate::SetInventoryType(uint32 value) {
    info->InventoryType = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetAllowableClass() { return info->AllowableClass; };
void CustomItemTemplate::SetAllowableClass(uint32 value) {
    info->AllowableClass = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetAllowableRace() { return info->AllowableRace; };
void CustomItemTemplate::SetAllowableRace(uint32 value) {
    info->AllowableRace = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetItemLevel() { return info->ItemLevel; };
void CustomItemTemplate::SetItemLevel(uint32 value) {
    info->ItemLevel = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetRequiredLevel() { return info->RequiredLevel; };
void CustomItemTemplate::SetRequiredLevel(uint32 value) {
    info->RequiredLevel = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetRequiredSkill() { return info->RequiredSkill; };
void CustomItemTemplate::SetRequiredSkill(uint32 value) {
    info->RequiredSkill = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetRequiredSkillRank() { return info->RequiredSkillRank; };
void CustomItemTemplate::SetRequiredSkillRank(uint32 value) {
    info->RequiredSkillRank = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetRequiredSpell() { return info->RequiredSpell; };
void CustomItemTemplate::SetRequiredSpell(uint32 value) {
    info->RequiredSpell = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetRequiredHonorRank() { return info->RequiredHonorRank; };
void CustomItemTemplate::SetRequiredHonorRank(uint32 value) {
    info->RequiredHonorRank = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetRequiredCityRank() { return info->RequiredCityRank; };
void CustomItemTemplate::SetRequiredCityRank(uint32 value) {
    info->RequiredCityRank = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetRequiredReputationFaction() { return info->RequiredReputationFaction; };
void CustomItemTemplate::SetRequiredReputationFaction(uint32 value) {
    info->RequiredReputationFaction = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetRequiredReputationRank() { return info->RequiredReputationRank; };
void CustomItemTemplate::SetRequiredReputationRank(uint32 value) {
    info->RequiredReputationRank = value;
    info->m_isDirty = true;
};
int32 CustomItemTemplate::GetMaxCount() { return info->MaxCount; };
void  CustomItemTemplate::SetMaxCount(int32 value) {
    info->MaxCount = value;
    info->m_isDirty = true;
};
int32 CustomItemTemplate::GetStackable() { return info->Stackable; };
void  CustomItemTemplate::SetStackable(int32 value) {
    info->Stackable = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetContainerSlots() { return info->ContainerSlots; };
void  CustomItemTemplate::SetContainerSlots(uint32 value) {
    info->ContainerSlots = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetStatsCount() { return info->StatsCount; };
void CustomItemTemplate::SetStatsCount(uint32 value) {
    info->StatsCount = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetScalingStatDistribution() { return info->ScalingStatDistribution; };
void CustomItemTemplate::SetScalingStatDistribution(uint32 value) {
    info->ScalingStatDistribution = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetScalingStatValue() { return info->ScalingStatValue; };
void CustomItemTemplate::SetScalingStatValue(uint32 value) {
    info->ScalingStatValue = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetArmor() { return info->Armor; };
void CustomItemTemplate::SetArmor(int32 value) {
    info->Armor = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetHolyRes() { return info->HolyRes; };
void CustomItemTemplate::SetHolyRes(int32 value) {
    info->HolyRes = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetFireRes() { return info->FireRes; };
void CustomItemTemplate::SetFireRes(int32 value) {
    info->FireRes = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetNatureRes() { return info->NatureRes; };
void CustomItemTemplate::SetNatureRes(int32 value) {
    info->NatureRes = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetFrostRes() { return info->FrostRes; };
void CustomItemTemplate::SetFrostRes(int32 value) {
    info->FrostRes = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetShadowRes() { return info->ShadowRes; };
void CustomItemTemplate::SetShadowRes(int32 value) {
    info->ShadowRes = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetArcaneRes() { return info->ArcaneRes; };
void CustomItemTemplate::SetArcaneRes(int32 value) {
    info->ArcaneRes = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetDelay() { return info->Delay; };
void CustomItemTemplate::SetDelay(uint32 value) {
    info->Delay = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetAmmoType() { return info->AmmoType; };
void CustomItemTemplate::SetAmmoType(uint32 value) {
    info->AmmoType = value;
    info->m_isDirty = true;
};
float CustomItemTemplate::GetRangedModRange() { return info->RangedModRange; };
void CustomItemTemplate::SetRangedModRange(float value) {
    info->RangedModRange = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetBonding() { return info->Bonding; };
void CustomItemTemplate::SetBonding(uint32 value) {
    info->Bonding = value;
    info->m_isDirty = true;
};
std::string CustomItemTemplate::GetDescription() { return info->Description; };
void CustomItemTemplate::SetDescription(std::string const& desc) {
    info->Description = desc;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetPageText() { return info->PageText; };
void CustomItemTemplate::SetPageText(uint32 value) {
    info->PageText = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetLanguageID() { return info->LanguageID; };
void CustomItemTemplate::SetLanguageID(uint32 value) {
    info->LanguageID = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetPageMaterial() { return info->PageMaterial; };
void CustomItemTemplate::SetPageMaterial(uint32 value) {
    info->PageMaterial = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetStartQuest() { return info->StartQuest; };
void CustomItemTemplate::SetStartQuest(uint32 value) {
    info->StartQuest = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetLockID() { return info->LockID; };
void CustomItemTemplate::SetLockID(uint32 value) {
    info->LockID = value;
    info->m_isDirty = true;
};
int32 CustomItemTemplate::GetMaterial() { return info->Material; };
void  CustomItemTemplate::SetMaterial(int32 value) {
    info->Material = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetSheath() { return info->Sheath; };
void CustomItemTemplate::SetSheath(uint32 value) {
    info->Sheath = value;
    info->m_isDirty = true;
};
int32 CustomItemTemplate::GetRandomProperty() { return info->RandomProperty; };
void  CustomItemTemplate::SetRandomProperty(int32 value) {
    info->RandomProperty = value;
    info->m_isDirty = true;
};
int32 CustomItemTemplate::GetRandomSuffix() { return info->RandomSuffix; };
void  CustomItemTemplate::SetRandomSuffix(int32 value) {
    info->RandomSuffix = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetBlock() { return info->Block; };
void CustomItemTemplate::SetBlock(uint32 value) {
    info->Block = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetItemSet() { return info->ItemSet; };
void CustomItemTemplate::SetItemSet(uint32 value) {
    info->ItemSet = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetMaxDurability() { return info->MaxDurability; };
void CustomItemTemplate::SetMaxDurability(uint32 value) {
    info->MaxDurability = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetArea() { return info->Area; };
void CustomItemTemplate::SetArea(uint32 value) {
    info->Area = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetMap() { return info->Map; };
void CustomItemTemplate::SetMap(uint32 value) {
    info->Map = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetBagFamily() { return info->BagFamily; };
void CustomItemTemplate::SetBagFamily(uint32 value) {
    info->BagFamily = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetTotemCategory() { return info->TotemCategory; };
void CustomItemTemplate::SetTotemCategory(uint32 value) {
    info->TotemCategory = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetSocketContent(uint32 index) { return info->Socket[index].Content; };
void CustomItemTemplate::SetSocketContent(uint32 index, uint32 value) {
    info->Socket[index].Content = value;
    info->m_isDirty = true;
}
uint32 CustomItemTemplate::GetSocketColor(uint32 index) { return info->Socket[index].Color; };
void CustomItemTemplate::SetSocketColor(uint32 index, uint32 value) {
    info->Socket[index].Color = value;
    info->m_isDirty = true;
}
uint32 CustomItemTemplate::GetSocketBonus() { return info->socketBonus; };
void CustomItemTemplate::SetSocketBonus(uint32 value) {
    info->socketBonus = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetGemProperties() { return info->GemProperties; };
void CustomItemTemplate::SetGemProperties(uint32 value) {
    info->GemProperties = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetRequiredDisenchantSkill() { return info->RequiredDisenchantSkill; };
void CustomItemTemplate::SetRequiredDisenchantSkill(uint32 value) {
    info->RequiredDisenchantSkill = value;
    info->m_isDirty = true;
};
float CustomItemTemplate::GetArmorDamageModifier() { return info->ArmorDamageModifier; };
void  CustomItemTemplate::SetArmorDamageModifier(float value) {
    info->ArmorDamageModifier = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetDuration() { return info->Duration; };
void CustomItemTemplate::SetDuration(uint32 value) {
    info->Duration = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetItemLimitCategory() { return info->ItemLimitCategory; };
void CustomItemTemplate::SetItemLimitCategory(uint32 value) {
    info->ItemLimitCategory = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetHolidayID() { return info->HolidayId; };
void CustomItemTemplate::SetHolidayID(uint32 value) {
    info->HolidayId = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetScriptID() { return info->ScriptId; };
void CustomItemTemplate::SetScriptID(uint32 value) {
    info->ScriptId = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetDisenchantID() { return info->DisenchantID; };
void CustomItemTemplate::SetDisenchantID(uint32 value) {
    info->DisenchantID = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetFoodType() { return info->FoodType; };
void CustomItemTemplate::SetFoodType(uint32 value) {
    info->FoodType = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetMinMoneyLoot() { return info->MinMoneyLoot; };
void CustomItemTemplate::SetMinMoneyLoot(uint32 value) {
    info->MinMoneyLoot = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetMaxMoneyLoot() { return info->MaxMoneyLoot; };
void CustomItemTemplate::SetMaxMoneyLoot(uint32 value) {
    info->MaxMoneyLoot = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetFlagsCu() { return info->FlagsCu; };
void CustomItemTemplate::SetFlagsCu(uint32 value) {
    info->FlagsCu = value;
    info->m_isDirty = true;
};
uint32 CustomItemTemplate::GetStatType(uint32 index) { return info->ItemStat[index].ItemStatType; }
void CustomItemTemplate::SetStatType(uint32 index, uint32 value) {
    info->ItemStat[index].ItemStatType = value;
    info->m_isDirty = true;
}
int32 CustomItemTemplate::GetStatValue(uint32 index) { return info->ItemStat[index].ItemStatValue; }
void CustomItemTemplate::SetStatValue(uint32 index, int32 value) {
    info->ItemStat[index].ItemStatValue = value;
    info->m_isDirty = true;
}
int32 CustomItemTemplate::GetStatValueMax(uint32 index) { return info->ItemStat[index].max; }
void CustomItemTemplate::SetStatValueMax(uint32 index, int32 value) {
    info->ItemStat[index].max = value;
    info->m_isDirty = true;
}
float CustomItemTemplate::GetDamageMinA() { return info->Damage[0].DamageMin; }
void CustomItemTemplate::SetDamageMinA(float value) {
    info->Damage[0].DamageMin = value;
    info->m_isDirty = true;
}
float CustomItemTemplate::GetDamageMinB() { return info->Damage[1].DamageMin; }
void CustomItemTemplate::SetDamageMinB(float value) {
    info->Damage[1].DamageMin = value;
    info->m_isDirty = true;
}
float CustomItemTemplate::GetDamageMaxA() { return info->Damage[0].DamageMax; }
void CustomItemTemplate::SetDamageMaxA(float value) {
    info->Damage[0].DamageMax = value;
    info->m_isDirty = true;
}
float CustomItemTemplate::GetDamageMaxB() { return info->Damage[1].DamageMax; }
void CustomItemTemplate::SetDamageMaxB(float value) {
    info->Damage[1].DamageMax = value;
    info->m_isDirty = true;
}

float CustomItemTemplate::GetMaxDamageMinA() { return info->Damage[0].maxDamageMin; }
void CustomItemTemplate::SetMaxDamageMinA(float value) {
    info->Damage[0].maxDamageMin = value;
    info->m_isDirty = true;
}
float CustomItemTemplate::GetMaxDamageMaxA() { return info->Damage[0].maxDamageMax; }
void CustomItemTemplate::SetMaxDamageMaxA(float value) {
    info->Damage[0].maxDamageMax = value;
    info->m_isDirty = true;
}

float CustomItemTemplate::GetMaxDamageMinB() { return info->Damage[1].maxDamageMin; }
void CustomItemTemplate::SetMaxDamageMinB(float value) {
    info->Damage[1].maxDamageMin = value;
    info->m_isDirty = true;
}
float CustomItemTemplate::GetMaxDamageMaxB() { return info->Damage[1].maxDamageMax; }
void CustomItemTemplate::SetMaxDamageMaxB(float value) {
    info->Damage[1].maxDamageMax = value;
    info->m_isDirty = true;
}

uint32 CustomItemTemplate::GetDamageTypeA() { return info->Damage[0].DamageType; }
void CustomItemTemplate::SetDamageTypeA(uint32 value) {
    info->Damage[0].DamageType = value;
    info->m_isDirty = true;
}
uint32 CustomItemTemplate::GetDamageTypeB() { return info->Damage[1].DamageType; }
void CustomItemTemplate::SetDamageTypeB(uint32 value) {
    info->Damage[1].DamageType = value;
    info->m_isDirty = true;
}
uint32 CustomItemTemplate::GetSpellCategory(uint32 index) { return info->Spells[index].SpellCategory; }
void CustomItemTemplate::SetSpellCategory(uint32 index, uint32 value) {
    info->Spells[index].SpellCategory = value;
    info->m_isDirty = true;
}
uint32 CustomItemTemplate::GetSpellCategoryCooldown(uint32 index) { return info->Spells[index].SpellCategoryCooldown; }
void CustomItemTemplate::SetSpellCategoryCooldown(uint32 index, uint32 value) {
    info->Spells[index].SpellCategoryCooldown = value;
    info->m_isDirty = true;
}
uint32 CustomItemTemplate::GetSpellCharges(uint32 index) { return info->Spells[index].SpellCharges; }
void CustomItemTemplate::SetSpellCharges(uint32 index, uint32 value) {
    info->Spells[index].SpellCharges = value;
    info->m_isDirty = true;
}
uint32 CustomItemTemplate::GetSpellCooldown(uint32 index) { return info->Spells[index].SpellCooldown; }
void CustomItemTemplate::SetSpellCooldown(uint32 index, uint32 value) {
    info->Spells[index].SpellCooldown = value;
    info->m_isDirty = true;
}
uint32 CustomItemTemplate::GetSpellID(uint32 index) { return info->Spells[index].SpellId; }
void CustomItemTemplate::SetSpellID(uint32 index, uint32 value) {
    info->Spells[index].SpellId = value;
    info->m_isDirty = true;
}
uint32 CustomItemTemplate::GetSpellPPMRate(uint32 index) { return info->Spells[index].SpellPPMRate; }
void CustomItemTemplate::SetSpellPPMRate(uint32 index, uint32 value) {
    info->Spells[index].SpellPPMRate = value;
    info->m_isDirty = true;
}
uint32 CustomItemTemplate::GetSpellTrigger(uint32 index) { return info->Spells[index].SpellTrigger; }
void CustomItemTemplate::SetSpellTrigger(uint32 index, uint32 value) {
    info->Spells[index].SpellTrigger = value;
    info->m_isDirty = true;
}
float CustomItemTemplate::GetItemSlotValue() { return info->ItemSlotValue; }
void CustomItemTemplate::SetItemSlotValue(float val) {
    info->ItemSlotValue = val;
    info->m_isDirty = true;
}

//extras
bool CustomItemTemplate::IsCurrencyToken() { return info->IsCurrencyToken(); }
uint32 CustomItemTemplate::GetMaxStackSize() {return info->GetMaxStackSize(); };
float CustomItemTemplate::GetDPS() { return info->getDPS(); };
bool CustomItemTemplate::CanChangeEquipStateInCombat() { return info->CanChangeEquipStateInCombat(); };
int32 CustomItemTemplate::GetFeralBonus(int32 extraDPS) { return info->getFeralBonus(extraDPS); }
int32 CustomItemTemplate::GetTotalAPBonus() {
    return info->GetTotalAPBonus(); 
}
float CustomItemTemplate::GetItemLevelIncludingQuality() {
    return info->GetItemLevelIncludingQuality();
};
uint32 CustomItemTemplate::GetSkill() { return info->GetSkill(); };
bool CustomItemTemplate::IsPotion() { return info->IsPotion(); };
bool CustomItemTemplate::IsWeaponVellum() { return info->IsWeaponVellum(); };
bool CustomItemTemplate::IsArmorVellum() { return info->IsArmorVellum(); };
bool CustomItemTemplate::IsConjuredConsumable() { return info->IsConjuredConsumable(); };
bool CustomItemTemplate::HasSignature() { return info->HasSignature(); };
//

CustomItemTemplate GetItemTemplate(uint32 entry)
{
    return CustomItemTemplate(sObjectMgr->GetItemTemplateMutable(entry));
}

ItemTemplate* CustomItemTemplate::_GetInfo()
{
    return info;
}

void CustomItemTemplate::AdjustForLevel(Player* player) {
    auto level = player->getLevel();

    auto ilvl = GetItemLevel();
    auto maxLvlScale = std::min(uint32(DEFAULT_MAX_LEVEL), ilvl);
    auto mainStat = ITEM_MOD_MANA;

    if (maxLvlScale >= level) {
        for (int i = 0; i < MAX_ITEM_PROTO_STATS; i++) {
            auto max = info->ItemStat[i].max;
            if (max) {
                switch (info->ItemStat[i].ItemStatType) {
                case ITEM_MOD_AGILITY:
                case ITEM_MOD_STRENGTH:
                case ITEM_MOD_INTELLECT:
                case ITEM_MOD_STAMINA:
                case ITEM_MOD_DEFENSE_SKILL_RATING:
                case ITEM_MOD_ATTACK_POWER:
                case ITEM_MOD_SPELL_POWER: {
                    float valPerLevel = float(max) / float(maxLvlScale);
                    auto newVal = valPerLevel * level;
                    SetStatValue(i, newVal);
                    break;
                }
                default:
                    break;
                }
            }
        }

        if (GetClass() == ITEM_CLASS_WEAPON) {
            auto max = (GetMaxDamageMaxA() / float(maxLvlScale)) * level;
            auto min = (GetMaxDamageMinA() / float(maxLvlScale)) * level;

            SetDamageMaxA(max);
            SetDamageMinA(min);
        } else if (GetClass() == ITEM_CLASS_ARMOR) {
            switch (GetSubClass()) {
            case ITEM_SUBCLASS_ARMOR_PLATE: {
                auto baseArmor = GetItemLevel() * 8.f;
                SetArmor(int32((baseArmor / float(maxLvlScale)) * level));
                break;
            }
            case ITEM_SUBCLASS_ARMOR_MAIL: {
                auto baseArmor = GetItemLevel() * 4.5f;
                SetArmor(int32((baseArmor / float(maxLvlScale)) * level));
                break;
            }
            case ITEM_SUBCLASS_ARMOR_LEATHER: {
                auto baseArmor = GetItemLevel() * 2.15;
                SetArmor(int32((baseArmor / float(maxLvlScale)) * level));
                break;
            }
            case ITEM_SUBCLASS_ARMOR_CLOTH: {
                auto baseArmor = GetItemLevel() * 1.08;
                SetArmor(int32((baseArmor / float(maxLvlScale)) * level));
                break;
            }
            case ITEM_SUBCLASS_ARMOR_SHIELD: {
                auto baseArmor = GetItemLevel() * 36.5f;
                SetArmor(int32((baseArmor / float(maxLvlScale)) * level));
                SetBlock((ilvl/float(maxLvlScale))*level);
            }}
        }

        Save();
        player->SendItemQueryPacket(this);
    }
}

float CustomItemTemplate::CalculateDps()
{
    std::mt19937 gen(rd());
    auto val = GetItemSlotValue();
    SetDamageTypeA(SPELL_SCHOOL_NORMAL);
    switch (GetInventoryType()) {
    case INVTYPE_WEAPON:
    case INVTYPE_WEAPONMAINHAND:
    case INVTYPE_WEAPONOFFHAND:
        return 0.711f * val + 4.2f;
        break;
    case INVTYPE_2HWEAPON:
        return 0.98f * val + 11.5f;
        break;
    case INVTYPE_RANGEDRIGHT:
        switch (GetSubClass()) {
        case ITEM_SUBCLASS_WEAPON_WAND: {
            std::uniform_int_distribution<> wandschoolroll(SPELL_SCHOOL_HOLY, SPELL_SCHOOL_ARCANE);
            SetDamageTypeA(wandschoolroll(gen));
            return  1.28f * val + 5.5f;
            break;
        }
        case ITEM_SUBCLASS_WEAPON_BOW:
        case ITEM_SUBCLASS_WEAPON_GUN:
        case ITEM_SUBCLASS_WEAPON_THROWN:
        case ITEM_SUBCLASS_WEAPON_CROSSBOW:
            return 0.98f * val + 21.5f;
            break;
        }
        break;
    default:
        return 0.f;
        break;
    }
    return 0.f;
}

ItemModType CustomItemTemplate::GenerateMainStatForItem()
{
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> agistrint(ITEM_MOD_AGILITY, ITEM_MOD_INTELLECT);
    std::uniform_int_distribution<> agistr(ITEM_MOD_AGILITY, ITEM_MOD_STRENGTH);
    std::uniform_int_distribution<> strint(ITEM_MOD_STRENGTH, ITEM_MOD_INTELLECT);
    if (IsWeapon()) {
        switch (GetSubClass()) {
        case ITEM_SUBCLASS_WEAPON_AXE:
        case ITEM_SUBCLASS_WEAPON_AXE2:
        case ITEM_SUBCLASS_WEAPON_MACE:
        case ITEM_SUBCLASS_WEAPON_MACE2:
        case ITEM_SUBCLASS_WEAPON_POLEARM:
        case ITEM_SUBCLASS_WEAPON_SWORD:
        case ITEM_SUBCLASS_WEAPON_SWORD2:
        case ITEM_SUBCLASS_WEAPON_STAFF:
        case ITEM_SUBCLASS_WEAPON_FIST:
        case ITEM_SUBCLASS_WEAPON_DAGGER: {
            return ItemModType(agistrint(gen));
        }
        case ITEM_SUBCLASS_WEAPON_THROWN:
        case ITEM_SUBCLASS_WEAPON_CROSSBOW:
        case ITEM_SUBCLASS_WEAPON_BOW:
        case ITEM_SUBCLASS_WEAPON_GUN: {
            return ItemModType(agistr(gen));
        }
        case ITEM_SUBCLASS_WEAPON_MISC:
        case ITEM_SUBCLASS_WEAPON_WAND:
            return ITEM_MOD_INTELLECT;
        }
    } else if (IsArmor()) {
        switch (GetSubClass())
        {
        case ITEM_SUBCLASS_ARMOR_CLOTH:
            return ITEM_MOD_INTELLECT;
        case ITEM_SUBCLASS_ARMOR_LEATHER:
            return ItemModType(agistr(gen));
        case ITEM_SUBCLASS_ARMOR_MAIL:
            return ItemModType(agistr(gen));
        case ITEM_SUBCLASS_ARMOR_PLATE:
        case ITEM_SUBCLASS_ARMOR_BUCKLER:
        case ITEM_SUBCLASS_ARMOR_SHIELD:
            return ItemModType(agistr(gen));
        }
    }
    return ItemModType(agistrint(gen));
}

bool CustomItemTemplate::CanRollStam()
{
    if (IsArmor()) {
        switch (GetSubClass()) {
        case ITEM_SUBCLASS_ARMOR_MISC:
        case ITEM_SUBCLASS_ARMOR_IDOL:
        case ITEM_SUBCLASS_ARMOR_TOTEM:
        case ITEM_SUBCLASS_ARMOR_LIBRAM:
        case ITEM_SUBCLASS_ARMOR_SIGIL:
            return false;
        default:
            return true;
        }
    }
    return true;
}

bool CustomItemTemplate::CanRollTank()
{
    if (IsWeapon()) {
        switch (GetInventoryType())
        {
        case INVTYPE_WEAPONOFFHAND:
            return false;
        default:
            break;
        }

        switch (GetSubClass()) {
        case ITEM_SUBCLASS_WEAPON_AXE:
        case ITEM_SUBCLASS_WEAPON_AXE2:
        case ITEM_SUBCLASS_WEAPON_MACE:
        case ITEM_SUBCLASS_WEAPON_MACE2:
        case ITEM_SUBCLASS_WEAPON_POLEARM:
        case ITEM_SUBCLASS_WEAPON_SWORD:
        case ITEM_SUBCLASS_WEAPON_SWORD2:
        case ITEM_SUBCLASS_WEAPON_STAFF:
        case ITEM_SUBCLASS_WEAPON_FIST:
        case ITEM_SUBCLASS_WEAPON_DAGGER: {
            return true;
        }
        case ITEM_SUBCLASS_WEAPON_THROWN:
        case ITEM_SUBCLASS_WEAPON_CROSSBOW:
        case ITEM_SUBCLASS_WEAPON_BOW:
        case ITEM_SUBCLASS_WEAPON_GUN: {
            return true;
        }
        case ITEM_SUBCLASS_WEAPON_MISC:
        case ITEM_SUBCLASS_WEAPON_WAND:
        default:
            return false;
        }
    }
    else if (IsArmor()) {
        switch (GetSubClass())
        {
        case ITEM_SUBCLASS_ARMOR_PLATE:
        case ITEM_SUBCLASS_ARMOR_BUCKLER:
        case ITEM_SUBCLASS_ARMOR_SHIELD:
            return true;
        case ITEM_SUBCLASS_ARMOR_CLOTH:
        case ITEM_SUBCLASS_ARMOR_LEATHER:
        case ITEM_SUBCLASS_ARMOR_MAIL:
        default:
            return false;
        }
    }
    return false;
}

void CustomItemTemplate::Save()
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CUSTOM_ITEM);
    stmt->SetData(0, info->ItemId);
    stmt->SetData(1, info->Class);
    stmt->SetData(2, info->SubClass);
    stmt->SetData(3, info->SoundOverrideSubclass);
    stmt->SetData(4, info->Name1);
    stmt->SetData(5, info->DisplayInfoID);
    stmt->SetData(6, info->Quality);
    stmt->SetData(7, info->Flags);
    stmt->SetData(8, info->Flags2);
    stmt->SetData(9, info->BuyCount);
    stmt->SetData(10, info->BuyPrice);
    stmt->SetData(11, info->SellPrice);
    stmt->SetData(12, info->InventoryType);
    stmt->SetData(13, int32(info->AllowableClass));
    stmt->SetData(14, int32(info->AllowableRace));
    stmt->SetData(15, info->ItemLevel);
    stmt->SetData(16, info->RequiredLevel);
    stmt->SetData(17, info->RequiredSkill);
    stmt->SetData(18, info->RequiredSkillRank);
    stmt->SetData(19, info->RequiredSpell);
    stmt->SetData(20, info->RequiredHonorRank);
    stmt->SetData(21, info->RequiredCityRank);
    stmt->SetData(22, info->RequiredReputationFaction);
    stmt->SetData(23, info->RequiredReputationRank);
    stmt->SetData(24, info->MaxCount);
    stmt->SetData(25, info->Stackable);
    stmt->SetData(26, info->ContainerSlots);
    stmt->SetData(27, info->StatsCount);
    stmt->SetData(28, info->ItemStat[0].ItemStatType);
    stmt->SetData(29, info->ItemStat[0].ItemStatValue);
    stmt->SetData(30, info->ItemStat[0].max);
    stmt->SetData(31, info->ItemStat[1].ItemStatType);
    stmt->SetData(32, info->ItemStat[1].ItemStatValue);
    stmt->SetData(33, info->ItemStat[1].max);
    stmt->SetData(34, info->ItemStat[2].ItemStatType);
    stmt->SetData(35, info->ItemStat[2].ItemStatValue);
    stmt->SetData(36, info->ItemStat[2].max);
    stmt->SetData(37, info->ItemStat[3].ItemStatType);
    stmt->SetData(38, info->ItemStat[3].ItemStatValue);
    stmt->SetData(39, info->ItemStat[3].max);
    stmt->SetData(40, info->ItemStat[4].ItemStatType);
    stmt->SetData(41, info->ItemStat[4].ItemStatValue);
    stmt->SetData(42, info->ItemStat[4].max);
    stmt->SetData(43, info->ItemStat[5].ItemStatType);
    stmt->SetData(44, info->ItemStat[5].ItemStatValue);
    stmt->SetData(45, info->ItemStat[5].max);
    stmt->SetData(46, info->ItemStat[6].ItemStatType);
    stmt->SetData(47, info->ItemStat[6].ItemStatValue);
    stmt->SetData(48, info->ItemStat[6].max);
    stmt->SetData(49, info->ItemStat[7].ItemStatType);
    stmt->SetData(50, info->ItemStat[7].ItemStatValue);
    stmt->SetData(51, info->ItemStat[7].max);
    stmt->SetData(52, info->ItemStat[8].ItemStatType);
    stmt->SetData(53, info->ItemStat[8].ItemStatValue);
    stmt->SetData(54, info->ItemStat[8].max);
    stmt->SetData(55, info->ItemStat[9].ItemStatType);
    stmt->SetData(56, info->ItemStat[9].ItemStatValue);
    stmt->SetData(57, info->ItemStat[9].max);
    stmt->SetData(58, info->ScalingStatDistribution);
    stmt->SetData(59, info->ScalingStatValue);
    stmt->SetData(60, info->Damage[0].DamageMin);
    stmt->SetData(61, info->Damage[0].DamageMax);
    stmt->SetData(62, info->Damage[0].DamageType);
    stmt->SetData(63, info->Damage[1].DamageMin);
    stmt->SetData(64, info->Damage[1].DamageMax);
    stmt->SetData(65, info->Damage[1].DamageType);
    stmt->SetData(66, info->Armor);
    stmt->SetData(67, info->HolyRes);
    stmt->SetData(68, info->FireRes);
    stmt->SetData(69, info->NatureRes);
    stmt->SetData(70, info->FrostRes);
    stmt->SetData(71, info->ShadowRes);
    stmt->SetData(72, info->ArcaneRes);
    stmt->SetData(73, info->Delay);
    stmt->SetData(74, info->AmmoType);
    stmt->SetData(75, info->RangedModRange);

    for (int i = 0; i < 5; ++i)
    {
        stmt->SetData( 76 + i * 7, info->Spells[i].SpellId);
        stmt->SetData( 77 + i * 7, info->Spells[i].SpellTrigger);
        stmt->SetData( 78 + i * 7, info->Spells[i].SpellCharges);
        stmt->SetData( 79 + i * 7, info->Spells[i].SpellPPMRate);
        stmt->SetData( 80 + i * 7, info->Spells[i].SpellCooldown);
        stmt->SetData( 81 + i * 7, info->Spells[i].SpellCategory);
        stmt->SetData( 82 + i * 7, info->Spells[i].SpellCategoryCooldown);
    }
    stmt->SetData(111, info->Bonding);
    stmt->SetData(112, info->Description);
    stmt->SetData(113, info->PageText);
    stmt->SetData(114, info->LanguageID);
    stmt->SetData(115, info->PageMaterial);
    stmt->SetData(116, info->StartQuest);
    stmt->SetData(117, info->LockID);
    stmt->SetData(118, info->Material);
    stmt->SetData(119, info->Sheath);
    stmt->SetData(120, info->RandomProperty);
    stmt->SetData(121, info->RandomSuffix);
    stmt->SetData(122, info->Block);
    stmt->SetData(123, info->ItemSet);
    stmt->SetData(124, info->MaxDurability);
    stmt->SetData(125, info->Area);
    stmt->SetData(126, info->Map);
    stmt->SetData(127, info->BagFamily);
    stmt->SetData(128, info->TotemCategory);
    stmt->SetData(129, info->Socket[0].Color);
    stmt->SetData(130, info->Socket[0].Content);
    stmt->SetData(131, info->Socket[1].Color);
    stmt->SetData(132, info->Socket[1].Content);
    stmt->SetData(133, info->Socket[2].Color);
    stmt->SetData(134, info->Socket[2].Content);
    stmt->SetData(135, info->socketBonus);
    stmt->SetData(136, info->GemProperties);
    stmt->SetData(137, int32(info->RequiredDisenchantSkill));
    stmt->SetData(138, info->ArmorDamageModifier);
    stmt->SetData(139, info->Duration);
    stmt->SetData(140, info->ItemLimitCategory);
    stmt->SetData(141, info->HolidayId);
    stmt->SetData(142, info->ScriptId);
    stmt->SetData(143, info->DisenchantID);
    stmt->SetData(144, info->FoodType);
    stmt->SetData(145, info->MinMoneyLoot);
    stmt->SetData(146, info->MaxMoneyLoot);
    stmt->SetData(147, info->FlagsCu);
    stmt->SetData(148, info->ItemSlotValue);
    stmt->SetData(149, info->Damage[0].maxDamageMin);
    stmt->SetData(150, info->Damage[0].maxDamageMax);
    stmt->SetData(151, info->Damage[1].maxDamageMin);
    stmt->SetData(152, info->Damage[1].maxDamageMax);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
}

bool CustomItemTemplate::IsWeapon() {
    return GetClass() == ItemClass::ITEM_CLASS_WEAPON;
}

bool CustomItemTemplate::IsArmor() {
    return GetClass() == ItemClass::ITEM_CLASS_ARMOR;
}

bool CustomItemTemplate::Is2hWeapon() {
    return GetClass() == ItemClass::ITEM_CLASS_WEAPON && (GetSubClass() == ITEM_SUBCLASS_WEAPON_AXE2 || GetSubClass() == ITEM_SUBCLASS_WEAPON_MACE2 || GetSubClass() == ITEM_SUBCLASS_WEAPON_POLEARM || GetSubClass() == ITEM_SUBCLASS_WEAPON_SWORD2 || GetSubClass() == ITEM_SUBCLASS_WEAPON_STAFF);
}

void CustomItemTemplate::MakeBlankSlate() {
    SetStatsCount(0);
    for (uint8 i = 0; i < MAX_ITEM_PROTO_STATS; ++i) {
        SetStatType(i, 0);
        SetStatValue(i, 0);
    }
    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i) {
        SetSpellID(i, 0);
        SetSpellTrigger(i, 0);
    }
    SetSocketBonus(0);
    for (uint8 i = 0; i < MAX_ITEM_PROTO_SOCKETS; ++i) {
        SetSocketColor(i, 0);
        SetSocketContent(i, 0);
    }

    if (IsWeapon()) {
        SetDamageMaxA(0);
        SetDamageMaxB(0);
        SetDamageMinA(0);
        SetDamageMinB(0);
        SetDamageTypeA(0);
        SetDamageTypeB(0);
    }

    SetFlags(GetFlags() & ~ITEM_FLAG_UNIQUE_EQUIPPABLE);
}

void CustomItemTemplate::InitializeQueryData()
{
    info->InitializeQueryData();
}
