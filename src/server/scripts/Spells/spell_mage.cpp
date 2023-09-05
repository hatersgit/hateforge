/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Scripts for spells with SPELLFAMILY_MAGE and SPELLFAMILY_GENERIC spells used by mage players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_mage_".
 */

#include "Pet.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "TemporarySummon.h"

enum MageSpells
{
    // Ours
    SPELL_MAGE_BURNOUT_TRIGGER                   = 44450,
    SPELL_MAGE_IMPROVED_BLIZZARD_CHILLED         = 12486,
    SPELL_MAGE_COMBUSTION                        = 11129,

    // Theirs
    SPELL_MAGE_COLD_SNAP                         = 11958,
    SPELL_MAGE_FOCUS_MAGIC_PROC                  = 54648,
    SPELL_MAGE_FROST_WARDING_R1                  = 11189,
    SPELL_MAGE_FROST_WARDING_TRIGGERED           = 57776,
    SPELL_MAGE_INCANTERS_ABSORBTION_R1           = 44394,
    SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED    = 44413,
    SPELL_MAGE_IGNITE                            = 12654,
    SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE       = 29077,
    SPELL_MAGE_SQUIRREL_FORM                     = 32813,
    SPELL_MAGE_GIRAFFE_FORM                      = 32816,
    SPELL_MAGE_SERPENT_FORM                      = 32817,
    SPELL_MAGE_DRAGONHAWK_FORM                   = 32818,
    SPELL_MAGE_WORGEN_FORM                       = 32819,
    SPELL_MAGE_SHEEP_FORM                        = 32820,
    SPELL_MAGE_GLYPH_OF_ETERNAL_WATER            = 70937,
    SPELL_MAGE_SUMMON_WATER_ELEMENTAL_PERMANENT  = 70908,
    SPELL_MAGE_SUMMON_WATER_ELEMENTAL_TEMPORARY  = 70907,
    SPELL_MAGE_GLYPH_OF_BLAST_WAVE               = 62126,
    SPELL_MAGE_FINGERS_OF_FROST                  = 44543,

    // Duskhaven
    SPELL_MAGE_BRAIN_FREEZE                      = 44549,
    SPELL_MAGE_BRAIN_FREEZE_AURA                 = 57761,
    SPELL_MAGE_CHAIN_REACTION                    = 1290055,
    SPELL_MAGE_CHAIN_REACTION_AURA               = 1290062,
    SPELL_MAGE_COMET_STORM_MISSILE_RANK1         = 1290026,
    SPELL_MAGE_COMET_STORM_MISSILE_RANK2         = 1290027,
    SPELL_MAGE_COMET_STORM_RANK1                 = 1290067,
    SPELL_MAGE_COMET_STORM_RANK2                 = 1290068,
    SPELL_MAGE_DIAMOND_ICE                       = 1290058,
    SPELL_MAGE_FINGERS_OF_FROST_AURA             = 1290012,
    SPELL_MAGE_FRACTURED_FROST_AURA              = 1290063,
    SPELL_MAGE_FRACTURED_FROST_RANK1             = 1290052,
    SPELL_MAGE_FRACTURED_FROST_RANK2             = 1290053,
    SPELL_MAGE_GLACIAL_SPIKE_RANK1               = 1290024,
    SPELL_MAGE_HAILSTONES_RANK1                  = 1290056,
    SPELL_MAGE_HAILSTONES_RANK2                  = 1290057,
    SPELL_MAGE_ICICLE_AURA                       = 1290061,
    SPELL_MAGE_ICICLE_RANK1                      = 1290029,
    SPELL_MAGE_ICICLE_RANK2                      = 1290030,
    SPELL_MAGE_ICICLE_RANK3                      = 1290031,
    SPELL_MAGE_ICICLE_RANK4                      = 1290032,
    SPELL_MAGE_ICICLE_RANK5                      = 1290033,
    SPELL_MAGE_ICICLE_RANK6                      = 1290034,
    SPELL_MAGE_ICICLE_RANK7                      = 1290035,
    SPELL_MAGE_ICICLE_RANK8                      = 1290036,
    SPELL_MAGE_ICICLE_RANK9                      = 1290037,
    SPELL_MAGE_ICY_VEINS                         = 12472,
    SPELL_MAGE_MASTERY_ICICLES                   = 1290060,
    SPELL_MAGE_PARADOX                           = 1290038,
    SPELL_MAGE_SNAP_FREEZE                       = 1290054,
    SPELL_MAGE_SPLINTERING_COLD_RANK1            = 1290047,
    SPELL_MAGE_SPLINTERING_COLD_RANK2            = 1290048,
    SPELL_MAGE_THERMAL_VOID                      = 1290059
};

enum SpellBunnies
{
    NPC_MAGE_COMET_STORM_TARGET                  = 1291000
};

class spell_mage_arcane_blast : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_blast);

    bool Load() override { _triggerSpellId = 0; return true; }

    void HandleTriggerSpell(SpellEffIndex effIndex)
    {
        _triggerSpellId = GetSpellInfo()->Effects[effIndex].TriggerSpell;
        PreventHitDefaultEffect(effIndex);
    }

    void HandleAfterCast()
    {
        GetCaster()->CastSpell(GetCaster(), _triggerSpellId, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_mage_arcane_blast::HandleTriggerSpell, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_arcane_blast::HandleTriggerSpell, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
        AfterCast += SpellCastFn(spell_mage_arcane_blast::HandleAfterCast);
    }

private:
    uint32 _triggerSpellId;
};

class spell_mage_burning_determination : public AuraScript
{
    PrepareAuraScript(spell_mage_burning_determination);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo() || !eventInfo.GetActionTarget())
            return false;

        // Need Interrupt or Silenced mechanic
        if (!(eventInfo.GetSpellInfo()->GetAllEffectsMechanicMask() & ((1 << MECHANIC_INTERRUPT) | (1 << MECHANIC_SILENCE))))
            return false;

        // Xinef: immuned effect should just eat charge
        if (eventInfo.GetHitMask() & PROC_EX_IMMUNE)
        {
            eventInfo.GetActionTarget()->RemoveAurasDueToSpell(54748);
            return false;
        }
        if (Aura* aura = eventInfo.GetActionTarget()->GetAura(54748))
        {
            if (aura->GetDuration() < aura->GetMaxDuration())
                eventInfo.GetActionTarget()->RemoveAurasDueToSpell(54748);
            return false;
        }

        return true;
    }

    void HandleProc(AuraEffect const*  /*aurEff*/, ProcEventInfo&  /*eventInfo*/)
    {
        PreventDefaultAction();
        GetUnitOwner()->CastSpell(GetUnitOwner(), 54748, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_burning_determination::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_burning_determination::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

class spell_mage_molten_armor : public AuraScript
{
    PrepareAuraScript(spell_mage_molten_armor);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo || (eventInfo.GetTypeMask() & PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK))
            return true;

        if (!eventInfo.GetActionTarget())
        {
            return false;
        }

        // Xinef: Molten Shields talent
        if (AuraEffect* aurEff = eventInfo.GetActionTarget()->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_MAGE, 16, EFFECT_0))
            return roll_chance_i(aurEff->GetSpellInfo()->GetRank() * 50);

        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_molten_armor::CheckProc);
    }
};

class spell_mage_mirror_image : public AuraScript
{
    PrepareAuraScript(spell_mage_mirror_image)

    void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell((Unit*)nullptr, GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell, true);
    }

    void CalcPeriodic(AuraEffect const* /*effect*/, bool& isPeriodic, int32&  /*amplitude*/)
    {
        isPeriodic = false;
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_mirror_image::HandleEffectApply, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_mage_mirror_image::CalcPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

class spell_mage_burnout : public AuraScript
{
    PrepareAuraScript(spell_mage_burnout);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_BURNOUT_TRIGGER });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo() != nullptr;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        int32 mana = int32(eventInfo.GetSpellInfo()->CalcPowerCost(GetTarget(), eventInfo.GetSchoolMask()));
        mana = CalculatePct(mana, aurEff->GetAmount());

        GetTarget()->CastCustomSpell(SPELL_MAGE_BURNOUT_TRIGGER, SPELLVALUE_BASE_POINT0, mana, GetTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_burnout::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_burnout::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

class spell_mage_burnout_trigger : public SpellScript
{
    PrepareSpellScript(spell_mage_burnout_trigger);

    void HandleDummy(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        if (Unit* target = GetHitUnit())
        {
            int32 newDamage = -(target->ModifyPower(POWER_MANA, -GetEffectValue()));
            GetSpell()->ExecuteLogEffectTakeTargetPower(effIndex, target, POWER_MANA, newDamage, 0.0f);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_burnout_trigger::HandleDummy, EFFECT_0, SPELL_EFFECT_POWER_BURN);
    }
};

class spell_mage_pet_scaling : public AuraScript
{
    PrepareAuraScript(spell_mage_pet_scaling);

    void CalculateResistanceAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: mage pet inherits 40% of resistance from owner and 35% of armor (guessed)
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            SpellSchoolMask schoolMask = SpellSchoolMask(aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
            int32 modifier = schoolMask == SPELL_SCHOOL_MASK_NORMAL ? 35 : 40;
            amount = CalculatePct(std::max<int32>(0, owner->GetResistance(schoolMask)), modifier);
        }
    }

    void CalculateStatAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: mage pet inherits 30% of intellect / stamina
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            Stats stat = Stats(aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
            amount = CalculatePct(std::max<int32>(0, owner->GetStat(stat)), 30);
        }
    }

    void CalculateAPAmount(AuraEffect const*  /*aurEff*/, int32&   /*amount*/, bool& /*canBeRecalculated*/)
    {
        // xinef: mage pet inherits 0% AP
    }

    void CalculateSPAmount(AuraEffect const*  /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: mage pet inherits 33% of SP
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            int32 frost = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FROST);
            amount = CalculatePct(std::max<int32>(0, frost), 33);

            // xinef: Update appropriate player field
            if (owner->GetTypeId() == TYPEID_PLAYER)
                owner->SetUInt32Value(PLAYER_PET_SPELL_POWER, (uint32)amount);
        }
    }

    void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (GetUnitOwner()->IsPet())
            return;

        GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, aurEff->GetAuraType(), true, SPELL_BLOCK_TYPE_POSITIVE);
        if (aurEff->GetAuraType() == SPELL_AURA_MOD_ATTACK_POWER)
            GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ATTACK_POWER_PCT, true, SPELL_BLOCK_TYPE_POSITIVE);
        else if (aurEff->GetAuraType() == SPELL_AURA_MOD_STAT)
            GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE, true, SPELL_BLOCK_TYPE_POSITIVE);
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        if (!GetUnitOwner()->IsPet())
            return;

        isPeriodic = true;
        amplitude = 2 * IN_MILLISECONDS;
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        PreventDefaultAction();
        if (aurEff->GetAuraType() == SPELL_AURA_MOD_STAT && (aurEff->GetMiscValue() == STAT_STAMINA || aurEff->GetMiscValue() == STAT_INTELLECT))
        {
            int32 currentAmount = aurEff->GetAmount();
            int32 newAmount = GetEffect(aurEff->GetEffIndex())->CalculateAmount(GetCaster());
            if (newAmount != currentAmount)
            {
                if (aurEff->GetMiscValue() == STAT_STAMINA)
                {
                    uint32 actStat = GetUnitOwner()->GetHealth();
                    GetEffect(aurEff->GetEffIndex())->ChangeAmount(newAmount, false);
                    GetUnitOwner()->SetHealth(std::min<uint32>(GetUnitOwner()->GetMaxHealth(), actStat));
                }
                else
                {
                    uint32 actStat = GetUnitOwner()->GetPower(POWER_MANA);
                    GetEffect(aurEff->GetEffIndex())->ChangeAmount(newAmount, false);
                    GetUnitOwner()->SetPower(POWER_MANA, std::min<uint32>(GetUnitOwner()->GetMaxPower(POWER_MANA), actStat));
                }
            }
        }
        else
            GetEffect(aurEff->GetEffIndex())->RecalculateAmount();
    }

    void Register() override
    {
        if (m_scriptSpellId != 35657)
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_pet_scaling::CalculateResistanceAmount, EFFECT_ALL, SPELL_AURA_MOD_RESISTANCE);

        if (m_scriptSpellId == 35657 || m_scriptSpellId == 35658)
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_pet_scaling::CalculateStatAmount, EFFECT_ALL, SPELL_AURA_MOD_STAT);

        if (m_scriptSpellId == 35657)
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_pet_scaling::CalculateAPAmount, EFFECT_ALL, SPELL_AURA_MOD_ATTACK_POWER);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_pet_scaling::CalculateSPAmount, EFFECT_ALL, SPELL_AURA_MOD_DAMAGE_DONE);
        }

        OnEffectApply += AuraEffectApplyFn(spell_mage_pet_scaling::HandleEffectApply, EFFECT_ALL, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_mage_pet_scaling::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_pet_scaling::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

class spell_mage_brain_freeze : public AuraScript
{
    PrepareAuraScript(spell_mage_brain_freeze);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return false;

        // xinef: Improved Blizzard, generic chilled check
        if (spellInfo->SpellFamilyFlags[0] & 0x100000)
            return spellInfo->Id == SPELL_MAGE_IMPROVED_BLIZZARD_CHILLED;

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_brain_freeze::CheckProc);
    }
};

class spell_mage_glyph_of_eternal_water : public AuraScript
{
    PrepareAuraScript(spell_mage_glyph_of_eternal_water);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (Player* player = target->ToPlayer())
                if (Pet* pet = player->GetPet())
                    if (pet->GetEntry() == NPC_WATER_ELEMENTAL_PERM)
                        pet->Remove(PET_SAVE_NOT_IN_SLOT);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_glyph_of_eternal_water::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

    class spell_mage_combustion_proc : public AuraScript
    {
        PrepareAuraScript(spell_mage_combustion_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_COMBUSTION });
    }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            GetTarget()->RemoveAurasDueToSpell(SPELL_MAGE_COMBUSTION);
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_mage_combustion_proc::OnRemove, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
        }
    };

// Incanter's Absorbtion
class spell_mage_incanters_absorbtion_base_AuraScript : public AuraScript
{
public:
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED, SPELL_MAGE_INCANTERS_ABSORBTION_R1 });
    }

    void Trigger(AuraEffect* aurEff, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
    {
        Unit* target = GetTarget();

        if (AuraEffect* talentAurEff = target->GetAuraEffectOfRankedSpell(SPELL_MAGE_INCANTERS_ABSORBTION_R1, EFFECT_0))
        {
            int32 bp = CalculatePct(absorbAmount, talentAurEff->GetAmount());
            if (AuraEffect* currentAura = target->GetAuraEffect(SPELL_AURA_MOD_DAMAGE_DONE, SPELLFAMILY_MAGE, 2941, EFFECT_0))
            {
                bp += int32(currentAura->GetAmount() * (currentAura->GetBase()->GetDuration() / (float)currentAura->GetBase()->GetMaxDuration()));
                currentAura->ChangeAmount(bp);
                currentAura->GetBase()->RefreshDuration();
            }
            else
                target->CastCustomSpell(target, SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED, &bp, nullptr, nullptr, true, nullptr, aurEff);
        }
    }
};

// -11113 - Blast Wave
class spell_mage_blast_wave : public SpellScript
{
    PrepareSpellScript(spell_mage_blast_wave);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_GLYPH_OF_BLAST_WAVE });
    }

    void HandleKnockBack(SpellEffIndex effIndex)
    {
        if (GetCaster()->HasAura(SPELL_MAGE_GLYPH_OF_BLAST_WAVE))
            PreventHitDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_blast_wave::HandleKnockBack, EFFECT_2, SPELL_EFFECT_KNOCK_BACK);
    }
};

// 11958 - Cold Snap
class spell_mage_cold_snap : public SpellScript
{
    PrepareSpellScript(spell_mage_cold_snap);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        // immediately finishes the cooldown on Frost spells

        PlayerSpellMap const& spellMap = caster->GetSpellMap();
        for (PlayerSpellMap::const_iterator itr = spellMap.begin(); itr != spellMap.end(); ++itr)
        {
            SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(itr->first);
            if (spellInfo->SpellFamilyName == SPELLFAMILY_MAGE && (spellInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_FROST) && ((spellInfo->SpellFamilyFlags[0] & 0x240) || (spellInfo->SpellFamilyFlags[1] & 0x81)) && spellInfo->GetRecoveryTime() > 0)
            {
                SpellCooldowns::iterator citr = caster->GetSpellCooldownMap().find(spellInfo->Id);
                if (citr != caster->GetSpellCooldownMap().end() && citr->second.needSendToClient)
                    caster->RemoveSpellCooldown(spellInfo->Id, true);
                else
                    caster->RemoveSpellCooldown(spellInfo->Id, false);
            }
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_cold_snap::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -543  - Fire Ward
// -6143 - Frost Ward
class spell_mage_fire_frost_ward : public spell_mage_incanters_absorbtion_base_AuraScript
{
    PrepareAuraScript(spell_mage_fire_frost_ward);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FROST_WARDING_TRIGGERED, SPELL_MAGE_FROST_WARDING_R1 });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            // +80.68% from sp bonus
            float bonus = 0.8068f;

            bonus *= caster->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask());
            bonus *= caster->CalculateLevelPenalty(GetSpellInfo());

            amount += int32(bonus);
        }
    }

    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* target = GetTarget();
        if (AuraEffect* talentAurEff = target->GetAuraEffectOfRankedSpell(SPELL_MAGE_FROST_WARDING_R1, EFFECT_0))
        {
            int32 chance = talentAurEff->GetSpellInfo()->Effects[EFFECT_1].CalcValue(); // SPELL_EFFECT_DUMMY with NO_TARGET

            if (roll_chance_i(chance))
            {
                int32 bp = dmgInfo.GetDamage();
                target->CastCustomSpell(target, SPELL_MAGE_FROST_WARDING_TRIGGERED, &bp, nullptr, nullptr, true, nullptr, aurEff);
                absorbAmount = 0;

                // Xinef: trigger Incanters Absorbtion
                uint32 damage = dmgInfo.GetDamage();
                Trigger(aurEff, dmgInfo, damage);

                // Xinef: hack for chaos bolt
                if (!dmgInfo.GetSpellInfo() || dmgInfo.GetSpellInfo()->SpellIconID != 3178)
                    dmgInfo.AbsorbDamage(bp);

                PreventDefaultAction();
            }
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_fire_frost_ward::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_fire_frost_ward::Absorb, EFFECT_0);
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_fire_frost_ward::Trigger, EFFECT_0);
    }
};

// 54646 - Focus Magic
class spell_mage_focus_magic : public AuraScript
{
    PrepareAuraScript(spell_mage_focus_magic);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FOCUS_MAGIC_PROC });
    }

    bool Load() override
    {
        _procTarget = nullptr;
        return true;
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        _procTarget = GetCaster();
        return _procTarget && _procTarget->IsAlive();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(_procTarget, SPELL_MAGE_FOCUS_MAGIC_PROC, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_focus_magic::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_focus_magic::HandleProc, EFFECT_0, SPELL_AURA_MOD_SPELL_CRIT_CHANCE);
    }

private:
    Unit* _procTarget;
};

// -11426 - Ice Barrier
class spell_mage_ice_barrier_aura : public spell_mage_incanters_absorbtion_base_AuraScript
{
    PrepareAuraScript(spell_mage_ice_barrier_aura);

    /// @todo: Rework
    static int32 CalculateSpellAmount(Unit* caster, int32 amount, SpellInfo const* spellInfo, const AuraEffect* aurEff)
    {
        // +80.68% from sp bonus
        float bonus = 0.8068f;

        bonus *= caster->SpellBaseDamageBonusDone(spellInfo->GetSchoolMask());

        // Glyph of Ice Barrier: its weird having a SPELLMOD_ALL_EFFECTS here but its blizzards doing :)
        // Glyph of Ice Barrier is only applied at the spell damage bonus because it was already applied to the base value in CalculateSpellDamage
        bonus = caster->ApplyEffectModifiers(spellInfo, aurEff->GetEffIndex(), bonus);

        bonus *= caster->CalculateLevelPenalty(spellInfo);

        amount += int32(bonus);
        return amount;
    }

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
            amount = CalculateSpellAmount(caster, amount, GetSpellInfo(), aurEff);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_ice_barrier_aura::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_ice_barrier_aura::Trigger, EFFECT_0);
    }
};

class spell_mage_ice_barrier : public SpellScript
{
    PrepareSpellScript(spell_mage_ice_barrier);

    /// @todo: Rework
    static int32 CalculateSpellAmount(Unit* caster, int32 amount, SpellInfo const* spellInfo, const AuraEffect* aurEff)
    {
        // +80.68% from sp bonus
        float bonus = 0.8068f;

        bonus *= caster->SpellBaseDamageBonusDone(spellInfo->GetSchoolMask());

        // Glyph of Ice Barrier: its weird having a SPELLMOD_ALL_EFFECTS here but its blizzards doing :)
        // Glyph of Ice Barrier is only applied at the spell damage bonus because it was already applied to the base value in CalculateSpellDamage
        bonus = caster->ApplyEffectModifiers(spellInfo, aurEff->GetEffIndex(), bonus);

        bonus *= caster->CalculateLevelPenalty(spellInfo);

        amount += int32(bonus);
        return amount;
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();

        if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_AURA_SCHOOL_ABSORB, (SpellFamilyNames)GetSpellInfo()->SpellFamilyName, GetSpellInfo()->SpellIconID, EFFECT_0))
        {
            int32 newAmount = GetSpellInfo()->Effects[EFFECT_0].CalcValue(caster, nullptr, nullptr);
            newAmount = CalculateSpellAmount(caster, newAmount, GetSpellInfo(), aurEff);

            if (aurEff->GetAmount() > newAmount)
                return SPELL_FAILED_AURA_BOUNCED;
        }

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_mage_ice_barrier::CheckCast);
    }
};

// -11119 - Ignite
class spell_mage_ignite : public AuraScript
{
    PrepareAuraScript(spell_mage_ignite);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_IGNITE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetActor() || !eventInfo.GetProcTarget())
            return false;

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();

        if (!damageInfo || !damageInfo->GetSpellInfo())
        {
            return false;
        }

        // Molten Armor
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
        {
            if (spellInfo->SpellFamilyFlags[1] & 0x8)
            {
                return false;
            }
        }

        return true;
    }

    void HandleProc(AuraEffect const*  /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        SpellInfo const* igniteDot = sSpellMgr->AssertSpellInfo(SPELL_MAGE_IGNITE);
        int32 pct = 8 * GetSpellInfo()->GetRank();

        float dmgRatio;
        int32 amount = int32(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), pct) / igniteDot->GetMaxTicks(eventInfo.GetActor(), dmgRatio));

        if (dmgRatio != 0)
            amount = amount * dmgRatio;

        // Xinef: implement ignite bug
        eventInfo.GetProcTarget()->CastDelayedSpellWithPeriodicAmount(eventInfo.GetActor(), SPELL_MAGE_IGNITE, SPELL_AURA_PERIODIC_DAMAGE, amount);
        //GetTarget()->CastCustomSpell(SPELL_MAGE_IGNITE, SPELLVALUE_BASE_POINT0, amount, eventInfo.GetProcTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_ignite::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_ignite::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -44457 - Living Bomb
class spell_mage_living_bomb : public AuraScript
{
    PrepareAuraScript(spell_mage_living_bomb);

    bool Validate(SpellInfo const* spell) override
    {
        if (!sSpellMgr->GetSpellInfo(uint32(spell->Effects[EFFECT_1].CalcValue())))
            return false;
        return true;
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
        if (removeMode != AURA_REMOVE_BY_ENEMY_SPELL && removeMode != AURA_REMOVE_BY_EXPIRE)
            return;

        if (Unit* caster = GetCaster())
            caster->CastSpell(GetTarget(), uint32(aurEff->GetAmount()), true, nullptr, aurEff);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_living_bomb::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// -1463 - Mana Shield
class spell_mage_mana_shield : public spell_mage_incanters_absorbtion_base_AuraScript
{
    PrepareAuraScript(spell_mage_mana_shield);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            // +80.53% from sp bonus
            float bonus = 0.8053f;

            bonus *= caster->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask());
            bonus *= caster->CalculateLevelPenalty(GetSpellInfo());

            amount += int32(bonus);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_mana_shield::CalculateAmount, EFFECT_0, SPELL_AURA_MANA_SHIELD);
        AfterEffectManaShield += AuraEffectManaShieldFn(spell_mage_mana_shield::Trigger, EFFECT_0);
    }
};

// -29074 - Master of Elements
class spell_mage_master_of_elements : public AuraScript
{
    PrepareAuraScript(spell_mage_master_of_elements);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE });
    }

    bool AfterCheckProc(ProcEventInfo& eventInfo, bool isTriggeredAtSpellProcEvent)
    {
        if (!isTriggeredAtSpellProcEvent || !eventInfo.GetActor() || !eventInfo.GetActionTarget())
        {
            return false;
        }

        _spellInfo = eventInfo.GetSpellInfo();

        bool selectCaster = false;
        // Triggered spells cost no mana so we need triggering spellInfo
        if (SpellInfo const* triggeredByAuraSpellInfo = eventInfo.GetTriggerAuraSpell())
        {
            _spellInfo = triggeredByAuraSpellInfo;
            selectCaster = true;
        }

        if (!_spellInfo)
        {
            return false;
        }

        _ticksModifier = 1;

        // If spell is periodic, mana amount is divided by tick number
        if (eventInfo.GetTriggerAuraEffectIndex() >= EFFECT_0)
        {
            if (Unit* caster = GetCaster())
            {
                if (Unit* target = (selectCaster ? eventInfo.GetActor() : eventInfo.GetActionTarget()))
                {
                    if (AuraEffect const* aurEff = target->GetAuraEffect(_spellInfo->Id, eventInfo.GetTriggerAuraEffectIndex(), caster->GetGUID()))
                    {
                        float f;
                        _ticksModifier = aurEff->GetTotalTicks(f);
                    }
                }
            }
        }

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        if (!_spellInfo)
            return;

        if (Unit* target = GetTarget())
        {
            int32 mana = int32(_spellInfo->CalcPowerCost(target, eventInfo.GetSchoolMask()) / _ticksModifier);
            mana = CalculatePct(mana, aurEff->GetAmount());

            if (mana > 0)
            {
                target->CastCustomSpell(SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE, SPELLVALUE_BASE_POINT0, mana, target, true, nullptr, aurEff);
            }
        }
    }

    void Register() override
    {
        DoAfterCheckProc += AuraAfterCheckProcFn(spell_mage_master_of_elements::AfterCheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_master_of_elements::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    SpellInfo const* _spellInfo = nullptr;
    uint8 _ticksModifier = 1;
};

enum SilvermoonPolymorph
{
    NPC_AUROSALIA   = 18744,
};

/// @todo move out of here and rename - not a mage spell
// 32826 - Polymorph (Visual)
class spell_mage_polymorph_cast_visual : public SpellScript
{
    PrepareSpellScript(spell_mage_polymorph_cast_visual);

    static const uint32 PolymorhForms[6];

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        // check if spell ids exist in dbc
        for (uint32 i = 0; i < 6; ++i)
            if (!sSpellMgr->GetSpellInfo(PolymorhForms[i]))
                return false;
        return true;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetCaster()->FindNearestCreature(NPC_AUROSALIA, 30.0f))
            if (target->GetTypeId() == TYPEID_UNIT)
                target->CastSpell(target, PolymorhForms[urand(0, 5)], true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_polymorph_cast_visual::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

const uint32 spell_mage_polymorph_cast_visual::spell_mage_polymorph_cast_visual::PolymorhForms[6] =
{
    SPELL_MAGE_SQUIRREL_FORM,
    SPELL_MAGE_GIRAFFE_FORM,
    SPELL_MAGE_SERPENT_FORM,
    SPELL_MAGE_DRAGONHAWK_FORM,
    SPELL_MAGE_WORGEN_FORM,
    SPELL_MAGE_SHEEP_FORM
};

// 31687 - Summon Water Elemental
class spell_mage_summon_water_elemental : public SpellScript
{
    PrepareSpellScript(spell_mage_summon_water_elemental)
    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_GLYPH_OF_ETERNAL_WATER,
                SPELL_MAGE_SUMMON_WATER_ELEMENTAL_TEMPORARY,
                SPELL_MAGE_SUMMON_WATER_ELEMENTAL_PERMANENT
            });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (Creature* pet = ObjectAccessor::GetCreature(*caster, caster->GetPetGUID()))
            if (!pet->IsAlive())
                pet->ToTempSummon()->UnSummon();

        // Glyph of Eternal Water
        if (caster->HasAura(SPELL_MAGE_GLYPH_OF_ETERNAL_WATER))
            caster->CastSpell(caster, SPELL_MAGE_SUMMON_WATER_ELEMENTAL_PERMANENT, true);
        else
            caster->CastSpell(caster, SPELL_MAGE_SUMMON_WATER_ELEMENTAL_TEMPORARY, true);

        if (Creature* pet = ObjectAccessor::GetCreature(*caster, caster->GetPetGUID()))
            if (pet->GetCharmInfo() && caster->ToPlayer())
            {
                pet->m_CreatureSpellCooldowns.clear();
                SpellInfo const* spellEntry = sSpellMgr->GetSpellInfo(31707);
                pet->GetCharmInfo()->ToggleCreatureAutocast(spellEntry, true);
                pet->GetCharmInfo()->SetSpellAutocast(spellEntry, true);
                caster->ToPlayer()->CharmSpellInitialize();
            }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_summon_water_elemental::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

#define FingersOfFrostScriptName "spell_mage_fingers_of_frost_proc_aura"
class spell_mage_fingers_of_frost_proc_aura : public AuraScript
{   PrepareAuraScript(spell_mage_fingers_of_frost_proc_aura);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellPhaseMask() != PROC_SPELL_PHASE_CAST)
        {
            eventInfo.SetProcChance(_chance);
        }

        return true;
    }

    bool AfterCheckProc(ProcEventInfo& eventInfo, bool isTriggeredAtSpellProcEvent)
    {
        if (eventInfo.GetSpellPhaseMask() != PROC_SPELL_PHASE_CAST)
        {
            eventInfo.ResetProcChance();
        }

        return isTriggeredAtSpellProcEvent;
    }

    void HandleOnEffectProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellPhaseMask() == PROC_SPELL_PHASE_CAST)
        {
            _chance = 100.f;
            _spell = eventInfo.GetProcSpell();

            if (!_spell || _spell->GetDelayMoment() <= 0)
            {
                PreventDefaultAction();
            }
        }
        else
        {
            if (eventInfo.GetSpellPhaseMask() == PROC_SPELL_PHASE_FINISH || ((_spell && _spell->GetDelayMoment() > 0) || !eventInfo.GetDamageInfo()))
            {
                PreventDefaultAction();
            }

            _chance = 0.f;
            _spell = nullptr;
        }
    }

    void HandleAfterEffectProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellPhaseMask() == PROC_SPELL_PHASE_HIT)
        {
            _chance = 100.f;
        }
        else if (eventInfo.GetSpellPhaseMask() == PROC_SPELL_PHASE_FINISH)
        {
            _chance = 0.f;
            _spell = nullptr;
        }
    }

    void Register()
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_fingers_of_frost_proc_aura::CheckProc);
        DoAfterCheckProc += AuraAfterCheckProcFn(spell_mage_fingers_of_frost_proc_aura::AfterCheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_fingers_of_frost_proc_aura::HandleOnEffectProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        AfterEffectProc += AuraEffectProcFn(spell_mage_fingers_of_frost_proc_aura::HandleAfterEffectProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }

public:
    Spell const* GetProcSpell() const { return _spell; }

private:
    float _chance = 0.f;
    Spell const* _spell = nullptr;
};

typedef spell_mage_fingers_of_frost_proc_aura spell_mage_fingers_of_frost_proc_aura_script;

class spell_mage_fingers_of_frost_proc : public AuraScript
{
    PrepareAuraScript(spell_mage_fingers_of_frost_proc);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (Aura* aura = GetCaster()->GetAuraOfRankedSpell(SPELL_MAGE_FINGERS_OF_FROST))
        {
            if (spell_mage_fingers_of_frost_proc_aura_script* script = dynamic_cast<spell_mage_fingers_of_frost_proc_aura_script*>(aura->GetScriptByName(FingersOfFrostScriptName)))
            {
                if (Spell const* fofProcSpell = script->GetProcSpell())
                {
                    if (fofProcSpell == eventInfo.GetProcSpell())
                    {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    void Register()
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_fingers_of_frost_proc::CheckProc);
    }
};

// Duskhaven
class spell_mage_icy_veins : public SpellScript
{
    PrepareSpellScript(spell_mage_icy_veins);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_SNAP_FREEZE,
                SPELL_MAGE_FINGERS_OF_FROST,
                SPELL_MAGE_FINGERS_OF_FROST_AURA,
                SPELL_MAGE_BRAIN_FREEZE,
                SPELL_MAGE_BRAIN_FREEZE_AURA
            });
    }

    void HandleDummy(SpellEffIndex)
    {
        Unit* caster = GetCaster();

        if (caster->HasAura(SPELL_MAGE_SNAP_FREEZE))
        {
            if (caster->HasAura(SPELL_MAGE_FINGERS_OF_FROST))
                caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST_AURA, true);
            if (caster->HasAura(SPELL_MAGE_BRAIN_FREEZE))
                caster->CastSpell(caster, SPELL_MAGE_BRAIN_FREEZE_AURA, true);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_icy_veins::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
    }
};

class spell_mage_cone_of_cold : public SpellScript
{
    PrepareSpellScript(spell_mage_cone_of_cold);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_DIAMOND_ICE
            });
    }

    void HandleDummy(SpellEffIndex)
    {
        Unit* caster = GetCaster();
        Player* player = caster->ToPlayer();

        if (caster->HasAura(SPELL_MAGE_DIAMOND_ICE))
        {
            player->RemoveCategoryCooldown(29); // Blizzard
            player->RemoveCategoryCooldown(58); // Comet Storm
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_cone_of_cold::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
    }
};

class spell_mage_frostfire_bolt : public SpellScript
{
    PrepareSpellScript(spell_mage_frostfire_bolt);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_PARADOX,
                SPELL_MAGE_BRAIN_FREEZE_AURA,
                SPELL_MAGE_FINGERS_OF_FROST_AURA
            });
    }

    void HandleCast()
    {
        Unit* caster = GetCaster();
        Unit* victim = caster->GetTargetUnit();

        if (caster->HasAura(SPELL_MAGE_PARADOX) && caster->HasAura(SPELL_MAGE_BRAIN_FREEZE_AURA))
            caster->CastSpell(victim, SPELL_MAGE_FINGERS_OF_FROST_AURA, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_frostfire_bolt::HandleCast);
    }
};

class spell_mage_frostbolt : public SpellScript
{
    PrepareSpellScript(spell_mage_frostbolt);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_SPLINTERING_COLD_RANK1,
                SPELL_MAGE_SPLINTERING_COLD_RANK2,
                SPELL_MAGE_ICICLE_AURA
            });
    }

    void HandleCast()
    {
        Unit* caster = GetCaster();

        if (caster->GetAura(SPELL_MAGE_SPLINTERING_COLD_RANK2) && (irand(1, 100) > 70))
            caster->CastSpell(caster, SPELL_MAGE_ICICLE_AURA, true);
        else if (caster->GetAura(SPELL_MAGE_SPLINTERING_COLD_RANK1) && (irand(1, 100) > 85))
            caster->CastSpell(caster, SPELL_MAGE_ICICLE_AURA, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_frostbolt::HandleCast);
    }
};

class spell_mage_frostbolt_trigger : public SpellScript
{
    PrepareSpellScript(spell_mage_frostbolt_trigger);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_FRACTURED_FROST_RANK1,
                SPELL_MAGE_FRACTURED_FROST_RANK2,
                SPELL_MAGE_FRACTURED_FROST_AURA
            });
    }

    void HandleBeforeCast()
    {
        Unit* caster = GetCaster();

        if (caster->HasAura(SPELL_MAGE_FRACTURED_FROST_RANK2) && (irand(1, 100) > 70))
            caster->CastSpell(caster, SPELL_MAGE_FRACTURED_FROST_AURA, true);
        else if (caster->HasAura(SPELL_MAGE_FRACTURED_FROST_RANK1) && (irand(1, 100) > 85))
            caster->CastSpell(caster, SPELL_MAGE_FRACTURED_FROST_AURA, true);
    }

    void HandleCast()
    {
        Unit* caster = GetCaster();

        if (caster->GetAura(SPELL_MAGE_FRACTURED_FROST_AURA))
            caster->RemoveAuraFromStack(SPELL_MAGE_FRACTURED_FROST_AURA);
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_mage_frostbolt_trigger::HandleBeforeCast);
        OnCast += SpellCastFn(spell_mage_frostbolt_trigger::HandleCast);
    }
};

class spell_mage_glacial_spike : public SpellScript
{
    PrepareSpellScript(spell_mage_glacial_spike);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ICICLE_AURA,
                _spellrank
            });
    }

public:
    spell_mage_glacial_spike(uint32 spellrank) : SpellScript(),
        _spellrank(spellrank) { }

    void HandleDamage(SpellMissInfo missInfo)
    {
        if (missInfo != SPELL_MISS_NONE)
        {
            return;
        }

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(_spellrank);
        int32 damage = GetHitDamage();
        damage += (spellInfo->GetEffect(EFFECT_0).CalcValue()) * 5;

        SetHitDamage(damage);
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_mage_glacial_spike::HandleDamage);
    }

private:
    uint32 _spellrank;
};

class spell_mage_glacial_spike_trigger : public SpellScript
{
    PrepareSpellScript(spell_mage_glacial_spike_trigger);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ICICLE_AURA
            });
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        int stacks = 0;

        if (caster->HasAura(SPELL_MAGE_ICICLE_AURA))
            stacks = caster->GetAura(SPELL_MAGE_ICICLE_AURA)->GetStackAmount();

        if (stacks < 5)
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        return SPELL_CAST_OK;
    }

    void HandleStacks()
    {
        Unit* caster = GetCaster();

        for (int i = 0; i < 5; i++)
        {
            caster->RemoveAuraFromStack(SPELL_MAGE_ICICLE_AURA);
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_mage_glacial_spike_trigger::CheckCast);
        OnCast += SpellCastFn(spell_mage_glacial_spike_trigger::HandleStacks);
    }
};

class SpellMageIceLanceEvent : public BasicEvent
{
public:
    SpellMageIceLanceEvent(Unit* caster, Unit* victim, uint32 spellid) : _caster(caster), _victim(victim), _spellid(spellid) {}

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        _caster->CastSpell(_victim, _spellid);
        return true;
    }

private:
    Unit* _caster;
    Unit* _victim;
    uint32 _spellid;
};

class spell_mage_ice_lance : public SpellScript
{
    PrepareSpellScript(spell_mage_ice_lance);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_FINGERS_OF_FROST_AURA,
                SPELL_MAGE_ICICLE_AURA,
                SPELL_MAGE_GLACIAL_SPIKE_RANK1,
                SPELL_MAGE_ICY_VEINS,
                SPELL_MAGE_THERMAL_VOID,
                SPELL_MAGE_HAILSTONES_RANK1,
                SPELL_MAGE_HAILSTONES_RANK2,
                SPELL_MAGE_CHAIN_REACTION,
                SPELL_MAGE_CHAIN_REACTION_AURA,
                _spellrank
            });
    }

public:
    spell_mage_ice_lance(uint32 spellrank) : SpellScript(),
        _spellrank(spellrank) { }

    void HandleDummy(SpellEffIndex)
    {
        Unit* caster = GetCaster();
        Unit* victim = caster->GetTargetUnit();

        if (caster->HasAura(SPELL_MAGE_ICICLE_AURA) && !caster->HasSpell(SPELL_MAGE_GLACIAL_SPIKE_RANK1))
        {
            Aura* icicleStack = caster->GetAura(SPELL_MAGE_ICICLE_AURA);
            int stacks = icicleStack->GetStackAmount();

            for (uint8 i = 0; i < stacks; i++)
            {
                caster->RemoveAuraFromStack(SPELL_MAGE_ICICLE_AURA);
                caster->m_Events.AddEvent(new SpellMageIceLanceEvent(caster, victim, _spellrank), caster->m_Events.CalculateTime(i*200));
            }
        }

        if (victim->HasAuraState(AURA_STATE_FROZEN) || caster->HasAura(SPELL_MAGE_FINGERS_OF_FROST_AURA))
        {
            if (caster->HasAura(SPELL_MAGE_CHAIN_REACTION))
                caster->CastSpell(caster, SPELL_MAGE_CHAIN_REACTION_AURA, true);

            if (caster->HasAura(SPELL_MAGE_ICY_VEINS) && caster->HasAura(SPELL_MAGE_THERMAL_VOID))
            {
                Aura* icyVeins = caster->GetAura(SPELL_MAGE_ICY_VEINS);
                icyVeins->AddDuration(1000);
            }

            if (caster->HasAura(SPELL_MAGE_HAILSTONES_RANK2))
                caster->CastSpell(caster, SPELL_MAGE_ICICLE_AURA, true);
            else if (caster->HasAura(SPELL_MAGE_HAILSTONES_RANK1) && irand(1, 100) > 50)
                caster->CastSpell(caster, SPELL_MAGE_ICICLE_AURA, true);
        }

        if (caster->HasAura(SPELL_MAGE_FINGERS_OF_FROST_AURA))
            caster->RemoveAuraFromStack(SPELL_MAGE_FINGERS_OF_FROST_AURA);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_ice_lance::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }

private:
    uint32 _spellrank;
};

class SpellMageCometStormEvent : public BasicEvent
{
public:
    SpellMageCometStormEvent(Unit* caster, Unit* victim, uint32 spellid, Position const& dest) : _caster(caster), _victim(victim), _spellid(spellid), _dest(dest), _count(0) {}

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        float x = _dest.GetPositionX() + frand(-3.0f, 3.0f);
        float y = _dest.GetPositionY() + frand(-3.0f, 3.0f);
        float z = _dest.GetPositionZ();
        
        Creature* pSpawn = _victim->SummonCreature(NPC_MAGE_COMET_STORM_TARGET, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 1500);
        pSpawn->SetFaction(_victim->GetFaction());
        _caster->CastSpell(pSpawn, _spellid, true);
        _count++;

        if (_count >= 7)
            return true;

        _caster->m_Events.AddEvent(this, irand(100, 275));
        return false;
    }

private:
    Unit* _caster;
    Unit* _victim;
    uint32 _spellid;
    uint8 _count;
    Position _dest;
};

class spell_mage_comet_storm : public SpellScript
{
    PrepareSpellScript(spell_mage_comet_storm);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                _spellrank
            });
    }

public:
    spell_mage_comet_storm(uint32 spellrank) : SpellScript(),
        _spellrank(spellrank) { }

    void EffectHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* victim = caster->GetTargetUnit();
        caster->m_Events.AddEventAtOffset(new SpellMageCometStormEvent(caster, victim, _spellrank, victim->GetPosition()/**GetHitDest()*/), randtime(100ms, 275ms));
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_comet_storm::EffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }

private:
    uint32 _spellrank;
};

void AddSC_mage_spell_scripts()
{
    RegisterSpellScript(spell_mage_arcane_blast);
    RegisterSpellScript(spell_mage_burning_determination);
    RegisterSpellScript(spell_mage_molten_armor);
    RegisterSpellScript(spell_mage_mirror_image);
    RegisterSpellScript(spell_mage_burnout);
    RegisterSpellScript(spell_mage_burnout_trigger);
    RegisterSpellScript(spell_mage_pet_scaling);
    RegisterSpellScript(spell_mage_brain_freeze);
    RegisterSpellScript(spell_mage_glyph_of_eternal_water);
    RegisterSpellScript(spell_mage_combustion_proc);
    RegisterSpellScript(spell_mage_blast_wave);
    RegisterSpellScript(spell_mage_cold_snap);
    RegisterSpellScript(spell_mage_fire_frost_ward);
    RegisterSpellScript(spell_mage_focus_magic);
    RegisterSpellScript(spell_mage_ice_barrier);
    RegisterSpellScript(spell_mage_ignite);
    RegisterSpellScript(spell_mage_living_bomb);
    RegisterSpellScript(spell_mage_mana_shield);
    RegisterSpellScript(spell_mage_master_of_elements);
    RegisterSpellScript(spell_mage_polymorph_cast_visual);
    RegisterSpellScript(spell_mage_summon_water_elemental);
    RegisterSpellScript(spell_mage_fingers_of_frost_proc_aura);    // Probably not needed anymore
    RegisterSpellScript(spell_mage_fingers_of_frost_proc);         // And this one as well
    
    // Duskhaven
    RegisterSpellScript(spell_mage_icy_veins);
    RegisterSpellScript(spell_mage_cone_of_cold);
    RegisterSpellScript(spell_mage_frostfire_bolt);
    RegisterSpellScript(spell_mage_frostbolt);
    RegisterSpellScript(spell_mage_frostbolt_trigger);
    RegisterSpellScriptWithArgs(spell_mage_glacial_spike, "spell_mage_glacial_spike_rank1", SPELL_MAGE_ICICLE_RANK8);
    RegisterSpellScriptWithArgs(spell_mage_glacial_spike, "spell_mage_glacial_spike_rank2", SPELL_MAGE_ICICLE_RANK9);
    RegisterSpellScript(spell_mage_glacial_spike_trigger);
    RegisterSpellScriptWithArgs(spell_mage_ice_lance, "spell_mage_ice_lance_rank1", SPELL_MAGE_ICICLE_RANK1);
    RegisterSpellScriptWithArgs(spell_mage_ice_lance, "spell_mage_ice_lance_rank2", SPELL_MAGE_ICICLE_RANK2);
    RegisterSpellScriptWithArgs(spell_mage_ice_lance, "spell_mage_ice_lance_rank3", SPELL_MAGE_ICICLE_RANK3);
    RegisterSpellScriptWithArgs(spell_mage_ice_lance, "spell_mage_ice_lance_rank4", SPELL_MAGE_ICICLE_RANK4);
    RegisterSpellScriptWithArgs(spell_mage_ice_lance, "spell_mage_ice_lance_rank5", SPELL_MAGE_ICICLE_RANK5);
    RegisterSpellScriptWithArgs(spell_mage_ice_lance, "spell_mage_ice_lance_rank6", SPELL_MAGE_ICICLE_RANK6);
    RegisterSpellScriptWithArgs(spell_mage_ice_lance, "spell_mage_ice_lance_rank7", SPELL_MAGE_ICICLE_RANK7);
    RegisterSpellScriptWithArgs(spell_mage_ice_lance, "spell_mage_ice_lance_rank8", SPELL_MAGE_ICICLE_RANK8);
    RegisterSpellScriptWithArgs(spell_mage_ice_lance, "spell_mage_ice_lance_rank9", SPELL_MAGE_ICICLE_RANK9);
    RegisterSpellScriptWithArgs(spell_mage_comet_storm, "spell_mage_comet_storm_rank1", SPELL_MAGE_COMET_STORM_MISSILE_RANK1);
    RegisterSpellScriptWithArgs(spell_mage_comet_storm, "spell_mage_comet_storm_rank2", SPELL_MAGE_COMET_STORM_MISSILE_RANK2);
}
